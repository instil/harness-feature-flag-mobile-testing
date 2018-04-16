// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "SurgeRtspPlayer_Private.h"
#import "SurgeLogging.h"
#import "SurgeH264Decoder.h"
#import "SurgeMp4vDecoder.h"
#import "SurgeMjpegDecoder.h"

#import "Surge.h"

#include "NSDate+SurgeExtensions.h"

class RtspLoggingDelegate: public SurgeUtil::ILoggingDelegate {
public:
    
    void trace(const char *message) const override {
        SurgeLogDebug(@"%s", message);
    }
    
    void debug(const char *message) const override {
        SurgeLogDebug(@"%s", message);
    }
    
    void info(const char *message) const override {
        SurgeLogInfo(@"%s", message);
    }
    
    void warn(const char *message) const override {
        SurgeLogWarn(@"%s", message);
    }
    
    void error(const char *message) const override {
        SurgeLogError(@"%s", message);
    }
    
    void fatal(const char *message) const override {
        SurgeLogError(@"%s", message);
    }
    
};

@protocol SurgeRtspClientDelegate <NSObject>
- (void)rtspClientDidTimeout;
- (void)rtspClientReceivedFrame:(const unsigned char *)frameBuffer
                         ofSize:(size_t)size
                 withDimensions:(CGSize)dimensions
               presentationTime:(unsigned int)presentationTime
                       duration:(int)duration;
@optional
- (void) rtspClientReceivedExtendedHeaders:(const unsigned char *)data
                                  ofLength:(size_t)length;

@end

class RtspClientDelegateWrapper : public Surge::IRtspClientDelegate {
public:
 
    RtspClientDelegateWrapper(id<SurgeRtspClientDelegate> delegate) : delegate(delegate) {}

    void ClientDidTimeout() {
        [delegate rtspClientDidTimeout];
    }
    
    void StreamConfigChanged(bool wasRedirect) {}

    void ClientReceivedFrame(const unsigned char* buffer,
                             size_t size,
                             int32_t width,
                             int32_t height,
                             int32_t presentationTime,
                             int32_t duration) {

        [delegate rtspClientReceivedFrame:buffer
                                   ofSize:size
                           withDimensions:CGSizeMake(width, height)
                         presentationTime:presentationTime
                                 duration:duration];
    }
    
    void ClientReceivedExtendedHeader(const unsigned char * buffer,
                                      size_t length) {
        if ([delegate respondsToSelector:@selector(rtspClientReceivedExtendedHeaders:ofLength:)]) {
            [delegate rtspClientReceivedExtendedHeaders:buffer ofLength:length];
        }
    }

private:
    __weak id<SurgeRtspClientDelegate> delegate;
};

@interface SurgeRtspPlayer () <SurgeRtspClientDelegate, SurgeDecoderDelegate>

@property (nonatomic, assign) Surge::IRtspClientDelegate *clientDelegate;
@property (nonatomic, strong) SurgeDecoder *decoder;

@end

@implementation SurgeRtspPlayer

+ (void)load {
    static RtspLoggingDelegate *delegate = new RtspLoggingDelegate();
    SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
    logger.SetLoggingDelegate(delegate);
}

- (instancetype)init {
    self = [super init];
    if (self) {
        self.clientDelegate = new RtspClientDelegateWrapper(self);
        self.client = new Surge::RtspClient(self.clientDelegate, self.interleavedTcpTransport);
        [self configurePlayerView];
        SurgeLogInfo("Initiating RTSP stream via Surge");
    }
    return self;
}

- (void)configurePlayerView {
#if TARGET_OS_IPHONE
    self.playerView = [[UIImageView alloc] init];
    self.playerView.backgroundColor = [UIColor blackColor];
    self.playerView.contentMode = UIViewContentModeScaleAspectFit;
#else
    self.playerView = [[NSImageView alloc] init];
    self.playerView.layer.backgroundColor = [NSColor blackColor].CGColor;
    self.playerView.imageScaling = NSImageScaleProportionallyUpOrDown;
#endif
}

- (void)dealloc {
    self.client->StopClient();
    delete self.client;
    delete self.clientDelegate;
}

- (void)closeStream {
    SurgeLogInfo(@"Closing Stream");
    
    self.client->StopStream();
}

#pragma mark - Public API

- (void)initiatePlaybackOf:(nonnull NSURL *)url {
    return [self initiatePlaybackOf:url withUsername:@"" andPassword:@""];
}

- (void)initiatePlaybackOf:(nonnull NSURL *)url withUsername:(nonnull NSString *)username andPassword:(nonnull NSString *)password {
    return [self initiatePlaybackOf:url withUsername:username andPassword:password startingAt:nil andEndingAt:nil];
}

- (void)initiatePlaybackOf:(nonnull NSURL *)url
              withUsername:(nonnull NSString *)username
               andPassword:(nonnull NSString *)password
                startingAt:(nullable NSDate *)startDate
               andEndingAt:(nullable NSDate *)endDate {
    SurgeLogDebug(@"Initating playback of %@", url);
    
    self.url = url;
    self.username = username;
    self.password = password;
    
    self.startTime = startDate;
    self.endTime = endDate;
    
    [self setRangeWithStartTime:startDate andEndTime:endDate];
    
    [self describeSetupPlay];
}

- (void)describeSetupPlay {

    __weak typeof(self) weakSelf = self;
    void(^onPlay)(RtspErrorCode) = ^(RtspErrorCode errorCode) {
        if (errorCode == 200) {
            if (weakSelf && [weakSelf.delegate respondsToSelector:@selector(rtspPlayerDidBeginPlayback:)]) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [weakSelf.delegate rtspPlayerDidBeginPlayback:weakSelf];
                });
            }
        } else {
            if ([weakSelf.delegate respondsToSelector:@selector(rtspPlayerFailedToInitiatePlayback:withErrorCode:)]) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [weakSelf.delegate rtspPlayerFailedToInitiatePlayback:weakSelf withErrorCode: (RtspErrorCode)errorCode];
                });
            }
        }
    };
    
    void(^onSetup)(RtspErrorCode) = ^(RtspErrorCode errorCode) {
        if (errorCode != RtspErrorCodeSuccess) {
            if (!weakSelf.interleavedRtspTransport) {
                SurgeLogInfo(@"Failed to connect to stream via UDP, trying Interleaved TCP");

                weakSelf.interleavedTcpTransport = true;
                [weakSelf describeSetupPlay];
            } else {
                if ([weakSelf.delegate respondsToSelector:@selector(rtspPlayerFailedToInitiatePlayback:withErrorCode:)]) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [weakSelf.delegate rtspPlayerFailedToInitiatePlayback:weakSelf withErrorCode: errorCode];
                    });
                }
                return;
            }
        }
        [weakSelf play:onPlay];
    };
    
    void(^onDescribe)(std::vector<Surge::SessionDescription>, RtspErrorCode) = ^(std::vector<Surge::SessionDescription> descriptions, RtspErrorCode errorCode) {
        weakSelf.sessionDescriptions = descriptions;
        if (errorCode != RtspErrorCodeSuccess || weakSelf.sessionDescriptions.size() == 0) {
            if ([weakSelf.delegate respondsToSelector:@selector(rtspPlayerFailedToInitiatePlayback:withErrorCode:)]) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [weakSelf.delegate rtspPlayerFailedToInitiatePlayback:weakSelf withErrorCode:errorCode];
                });
            }
            return;
        }
        else if (weakSelf && [weakSelf.delegate respondsToSelector:@selector(rtspPlayerInitiatedPlayback:)]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [weakSelf.delegate rtspPlayerInitiatedPlayback:weakSelf];
            });
        }
        Surge::SessionDescription currentSessionDescription = [weakSelf selectPreferredSessionDescription];
        [weakSelf setupStream:currentSessionDescription withCallback:onSetup];
    };
    
    [self describe:onDescribe];
}

- (void)seekToStartTime:(nullable NSDate *)startTime
             andEndTime:(nullable NSDate *)endTime {
    
    SurgeLogInfo(@"Seeking to start time %@ and end time %@", startTime, endTime);
    
    if ([startTime compare:endTime] == NSOrderedDescending) {
        SurgeLogError(@"End time cannot be earlier than Start time");
        return;
    }
    
    self.startTime = startTime;
    self.endTime = endTime;
    
    [self pause];
    
    [self setRangeWithStartTime:self.startTime
                     andEndTime:self.endTime];
    
    [self play];
}

- (void)setStreamToLiveVideo {
    SurgeLogInfo(@"Resetting stream back to live video");
    
    [self pause];
    
    self.client->ResetTimeToLive();

    [self play];
}

- (void)describe:(void (^)(std::vector<Surge::SessionDescription>, RtspErrorCode))callback {
    self.client->Describe(std::string(self.url.absoluteString.UTF8String),
                          std::string(self.username.UTF8String),
                          std::string(self.password.UTF8String),
                          [=](Surge::DescribeResponse *describeResponse) {
                              
                              std::vector<Surge::SessionDescription> descriptions = std::vector<Surge::SessionDescription>();
                              int errorCode = (int)RtspErrorCodeUnknownFailure;
                              
                              if (describeResponse != NULL) {
                                  descriptions = describeResponse->GetSessionDescriptions();
                                  errorCode = describeResponse->GetCode();
                                  delete describeResponse;
                              }
                              if (callback) {
                                  callback(descriptions, (RtspErrorCode)errorCode);
                              }
      });
}

- (void)setupStream:(Surge::SessionDescription)sessionDescription withCallback:(void (^)(RtspErrorCode))callback {
    SurgeLogInfo(@"Setting up stream with SessionDescription; %@",
                 [NSString stringWithUTF8String:sessionDescription.GetFmtp().c_str()]);
    
    [self initialiseDecoderForStream:sessionDescription];
    self.client->Setup(sessionDescription,
                       false,
                       [=](Surge::SetupResponse *setupResponse) {
                           if (setupResponse != nullptr) {
                               callback((RtspErrorCode)setupResponse->GetCode());
                               delete setupResponse;
                           } else {
                               callback(RtspErrorCodeUnknownFailure);
                           }
                       });
}

- (void)initialiseDecoderForStream:(Surge::SessionDescription)sessionDescription {
    if (sessionDescription.GetType() == Surge::RtspSessionType::H264) {
        self.decoder = [[SurgeH264Decoder alloc] initWithDelegate:self];
    } else if (sessionDescription.GetType() == Surge::RtspSessionType::MP4V) {
        self.decoder = [[SurgeMp4vDecoder alloc] initWithDelegate:self];
    } else if (sessionDescription.GetType() == Surge::RtspSessionType::MJPEG) {
        self.decoder = [[SurgeMjpegDecoder alloc] initWithDelegate:self];
    }
}

- (void)play:(void (^)(RtspErrorCode)) callback {
    SurgeLogInfo(@"Starting/resuming playback of %@", self.url);
    self.client->Play(false,
                      [=](Surge::RtspResponse *playResponse) {
                          if (callback) {
                              callback((RtspErrorCode)playResponse->GetCode());
                          } else {
                              callback(RtspErrorCodeUnknownFailure);
                          }
                          delete playResponse;
                      });
}

- (void)play {
    __weak typeof(self) weakSelf = self;
    void(^callback)(RtspErrorCode) = ^(RtspErrorCode errorCode) {
        if (errorCode == RtspErrorCodeSuccess && [weakSelf.delegate respondsToSelector:@selector(rtspPlayerDidBeginPlayback:)]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [weakSelf.delegate rtspPlayerDidBeginPlayback:weakSelf];
            });
        }
    };
    [self play:callback];
}

- (void)pause {
    SurgeLogInfo(@"Pausing playback of %@", self.url);
    __weak typeof(self) weakSelf = self;
    void(^callback)(bool) = ^(bool success) {
        if (success && [weakSelf.delegate respondsToSelector:@selector(rtspPlayerDidStopPlayback:)]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [weakSelf.delegate rtspPlayerDidStopPlayback:weakSelf];
            });
        }
    };
    self.client->Pause([=](Surge::RtspResponse *pauseResponse) {
        callback(pauseResponse->Ok());
        delete pauseResponse;
    });
}

- (void)stop {
    SurgeLogInfo(@"Stopping playback of %@", self.url);
    [self closeStream];
    self.client->StopClient();
    if ([self.delegate respondsToSelector:@selector(rtspPlayerDidStopPlayback:)]) {
        [self.delegate rtspPlayerDidStopPlayback:self];
    }
    
    [self.decoder deinit];
}

#pragma mark - Package API

- (void)setRangeWithStartTime:(nullable NSDate *)startTime
                   andEndTime:(nullable NSDate *)endTime {
    
    SurgeUtil::DateTime surgeStartTime;
    SurgeUtil::DateTime surgeEndTime;
    
    if (startTime != nil) {
        surgeStartTime = [startTime toSurgeDateTime];
        self.client->SetStartTime(surgeStartTime);
    } else {
        self.client->ClearStartTime();
    }
    
    if (endTime != nil) {
        surgeEndTime = [endTime toSurgeDateTime];
        self.client->SetEndTime(surgeEndTime);
    } else {
        self.client->ClearEndTime();
    }
}

- (Surge::SessionDescription)selectPreferredSessionDescription {
    return self.sessionDescriptions[0];
}

- (int) framesPerSecond {
//    SurgeLogDebug(@"Current FPS: %@", @(self.decoder.framesPerSecond));
    return (int)self.decoder.framesPerSecond;
}

- (int)timeout {
    return self.client->GetTimeout();
}

- (void)setTimeout:(int)timeout {
    self.client->SetTimeout(timeout);
}

- (bool)interleavedRtspTransport {
    return self.client->IsInterleavedTransport();
}

- (void)setInterleavedTcpTransport: (bool)usingInterleavedTcpTransport {
    if (usingInterleavedTcpTransport != self.client->IsInterleavedTransport()) {
        delete self.client;
        self.client = new Surge::RtspClient(self.clientDelegate, usingInterleavedTcpTransport);
    }
}

- (int)packetBufferDelay {
    return self.client->GetPacketBufferDelay();
}

- (void)setPacketBufferDelay: (int)packetBufferDelay {
    self.client->SetPacketBufferDelay(packetBufferDelay);
}


#pragma mark - RTSP client delegate

- (void)rtspClientDidTimeout {
    if ([self.delegate respondsToSelector:@selector(rtspPlayerDidTimeout:)]) {
        [self.delegate rtspPlayerDidTimeout:self];
    }
}

- (void)rtspClientReceivedFrame:(const unsigned char *)frameBuffer
                         ofSize:(size_t)size
                 withDimensions:(CGSize)dimensions
               presentationTime:(unsigned int)presentationTime
                       duration:(int)duration {

    [self.decoder decodeFrameBuffer:frameBuffer
                             ofSize:size
                     withDimensions:dimensions
                   presentationTime:presentationTime
                           duration:duration];
}

#pragma mark - Decoder delegate

- (void)decoderFrameAvailable:(CGImageRef)image withTimeStamp:(NSTimeInterval)timestamp {
    if (!self.playerView) {
        return;
    }
    dispatch_sync(dispatch_get_main_queue(), ^{
        #if TARGET_OS_IPHONE
            self.playerView.image = [[UIImage alloc] initWithCGImage:image];
        #else
            self.playerView.image = [[NSImage alloc] initWithCGImage:image size:NSZeroSize];
        #endif
    });
}

- (void)decoderFramerateUpdated:(NSInteger)framerate {
    if ([self.delegate respondsToSelector:@selector(rtspPlayer:didObservePlaybackFrameRate:)]) {
        [self.delegate rtspPlayer:self didObservePlaybackFrameRate:framerate];
    }
}

- (void)decoderDidBeginBuffering {
    if ([self.delegate respondsToSelector:@selector(rtspPlayerDidBeginBuffering:)]) {
        [self.delegate rtspPlayerDidBeginBuffering:self];
    }
}

- (void)decoderDidStopBuffering {
    if ([self.delegate respondsToSelector:@selector(rtspPlayerDidStopBuffering:)]) {
        [self.delegate rtspPlayerDidStopBuffering:self];
    }
}

@end
