// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#import "SurgeRtspPlayer.h"
#import "SurgeLogging.h"
#import "SurgeH264Decoder.h"

#include "Surge.h"
#include "MimeTypes.h"
#include "Logging.h"
#include "LoggingDelegate.h"

class RtspLoggingDelegate: public SurgeUtil::LoggingDelegate {
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
- (void)rtspClientPayloadReceived:(const unsigned char *)payload withSize:(size_t)length;
@end

class RtspClientDelegate : public Surge::RtspClientDelegate {
public:

    RtspClientDelegate(id<SurgeRtspClientDelegate> delegate) : delegate(delegate) {}

    void ClientDidTimeout() {
        [delegate rtspClientDidTimeout];
    }

    void Payload(const char* buffer, size_t length) {
        [delegate rtspClientPayloadReceived:(const unsigned char *)buffer withSize:length];
    }

private:
    id<SurgeRtspClientDelegate> delegate;
};

@interface SurgeRtspPlayer () <SurgeRtspClientDelegate, SurgeDecoderDelegate>
@property (nonatomic, strong) NSURL *url;
@property (nonatomic, strong) UIImageView *playerView;
@property (nonatomic, assign) Surge::RtspClientDelegate *clientDelegate;
@property (nonatomic, assign) Surge::RtspClient *client;
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
        self.playerView = [[UIImageView alloc] init];
        self.playerView.backgroundColor = [UIColor blackColor];
        self.playerView.contentMode = UIViewContentModeScaleAspectFit;
        self.clientDelegate = new RtspClientDelegate(self);
        self.client = new Surge::RtspClient(self.clientDelegate);
    }
    return self;
}

- (void)dealloc {
    self.client->StopClient();
    delete self.client;
    delete self.clientDelegate;
}

#pragma mark - Public API

- (void)initiatePlaybackOf:(NSURL*)url {
    SurgeLogDebug(@"Initating playback of %@", url);
    
    self.url = url;
    
    Surge::DescribeResponse *describeResponse = self.client->Describe(std::string(self.url.absoluteString.UTF8String), false, "", "");
    Surge::SessionDescription sessionDescription = describeResponse->GetPalettes()[0];
    [self initialiseDecoderForStream:sessionDescription];
    delete describeResponse;
    
    Surge::RtspResponse *setupResponse = self.client->Setup(sessionDescription, false);
    delete setupResponse;
    
    [self play];
}

- (void)initialiseDecoderForStream:(Surge::SessionDescription)sessionDescription {
    if (sessionDescription.GetType() == Surge::RtspSessionType::H264) {
        self.decoder = [[SurgeH264Decoder alloc] initWithDelegate:self];
    }
}

- (void)play {
    SurgeLogDebug(@"Starting playback of %@", self.url);
    Surge::RtspResponse *playResponse = self.client->Play(false);
    delete playResponse;
}

- (void)pause {
    SurgeLogDebug(@"Pausing playback of %@", self.url);
    Surge::RtspResponse *pauseResponse = self.client->Pause();
    delete pauseResponse;
}

- (void)stop {
    SurgeLogDebug(@"Stopping playback of %@", self.url);
    Surge::RtspResponse *teardownResponse = self.client->Teardown();
    delete teardownResponse;
}

#pragma mark - RTSP client delegate

- (void)rtspClientDidTimeout {
    [self.delegate rtspPlayerDidTimeout];
}

- (void)rtspClientPayloadReceived:(const unsigned char *)payload withSize:(size_t)length {
    [self.decoder decodeFrameBuffer:payload ofSize:length withFrameDuration:0 andPresentationTime:0];
}

#pragma mark - Decoder delegate

- (void)decoderFrameAvailable:(CGImageRef)imageBuffer withTimeStamp:(NSTimeInterval)timestamp {
    dispatch_sync(dispatch_get_main_queue(), ^{
        self.playerView.image = [[UIImage alloc] initWithCGImage:imageBuffer];
    });
}

- (void)decoderDidBeginBuffering {
    if ([self.delegate respondsToSelector:@selector(rtspPlayerDidBeginBuffering)]) {
        [self.delegate rtspPlayerDidBeginBuffering];
    }
}

- (void)decoderDidStopBuffering {
    if ([self.delegate respondsToSelector:@selector(rtspPlayerDidStopBuffering)]) {
        [self.delegate rtspPlayerDidStopBuffering];
    }
}

@end
