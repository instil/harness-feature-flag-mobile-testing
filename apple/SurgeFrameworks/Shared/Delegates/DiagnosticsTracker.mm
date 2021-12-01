// Copyright (c) 2018 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "DiagnosticsTracker.h"

#import "SurgeLogging.h"
#import "RtspClient.h"
#import "SurgeRtspPlayer_Private.h"

#import "RtpPacketBuffer.h"

@interface DiagnosticsTracker()

@property (nonatomic, readwrite) int framesPerSecond;
@property (nonatomic, readwrite) size_t bitrate;
@property (nonatomic, readwrite) long outOfOrderPacketsReceived;
@property (nonatomic, readwrite) long successfulPacketsReceived;
@property (nonatomic, readwrite) long lostPackets;
@property (nonatomic, readwrite) CGSize mediaDimensions;
@property (nonatomic, readwrite) int mediaFormat;

@property (nonatomic, weak) SurgeRtspPlayer *player;

@property (nonatomic, strong) dispatch_queue_t trackerQueue;
@property (nonatomic, strong) dispatch_source_t timer;
@property (nonatomic, readonly) BOOL timerIsRunning;

@property (nonatomic) int framePerSecondCounter;
@property (nonatomic) size_t bitrateCounter;

@end

@implementation DiagnosticsTracker

- (id)initWithPlayer:(nonnull SurgeRtspPlayer *)player {
    if (self = [super init]) {
        self.player = player;
        dispatch_queue_attr_t queueAttributes = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_SERIAL, QOS_CLASS_BACKGROUND, 0);
        self.trackerQueue = dispatch_queue_create("co.instil.diagnosticstracker", queueAttributes);
        self.framePerSecondCounter = 0;
    }
    return self;
}

- (BOOL)timerIsRunning {
    return self.timer != nil && dispatch_source_testcancel(self.timer) == 0;
}

- (void)startTracking {
    if (self.timerIsRunning) {
        return;
    }

    SurgeLogDebug(@"Starting diagnostics tracker");
    [self createTimer];
    dispatch_resume(self.timer);
}

- (void)createTimer {
    self.timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, self.trackerQueue);
    dispatch_source_set_timer(self.timer, dispatch_time(DISPATCH_TIME_NOW, 0), (int64_t)(1 * NSEC_PER_SEC), 0);
    __weak DiagnosticsTracker *weakSelf = self;
    dispatch_source_set_event_handler(self.timer, ^{
        [weakSelf updateDelegate];
    });
}

- (void)stopTracking {
    SurgeLogDebug(@"Stopping diagnostics tracker");
    dispatch_source_cancel(self.timer);
}

- (void)trackNewFrameOfSize:(size_t)bitCountInFrame {
    ++self.framePerSecondCounter;
    self.bitrateCounter += bitCountInFrame;
}

- (void)trackNewFrameDimensionsWithWidth:(size_t)width andHeight:(size_t)height {
    if (self.mediaDimensions.width != width && self.mediaDimensions.height != height) {
        self.mediaDimensions = CGSizeMake(width, height);

        if ([self.diagnosticsDelegate respondsToSelector:@selector(rtspPlayer:didChangeMediaSize:)]) {
            [self.diagnosticsDelegate rtspPlayer:self.player didChangeMediaSize:self.mediaDimensions];
        }
    }
}

- (void)trackNewMediaFormat:(int)mediaFormat {
    self.mediaFormat = mediaFormat;

    if ([self.diagnosticsDelegate respondsToSelector:@selector(rtspPlayer:didChangeMediaFormat:)]) {
        [self.diagnosticsDelegate rtspPlayer:self.player didChangeMediaFormat:self.mediaFormat];
    }
}

- (void)updateDelegate {
    self.framesPerSecond = self.framePerSecondCounter;
    self.framePerSecondCounter = 0;

    self.bitrate = self.bitrateCounter;
    self.bitrateCounter = 0;

    [self setPacketBufferInfo: self.player.client->GetDiagnosticsOnPacketLoss()];
}

- (void)setFramesPerSecond:(int)framesPerSecond {
    _framesPerSecond = framesPerSecond;

    if ([self.diagnosticsDelegate respondsToSelector:@selector(rtspPlayer:didObservePlaybackFrameRate:)]) {
        [self.diagnosticsDelegate rtspPlayer:self.player didObservePlaybackFrameRate:self.framesPerSecond];
    }
}

- (void)setBitrate:(size_t)bitrate {
    _bitrate = bitrate;
    if ([self.diagnosticsDelegate respondsToSelector:@selector(rtspPlayer:didObservePlaybackBitrate:)]) {
        [self.diagnosticsDelegate rtspPlayer:self.player didObservePlaybackBitrate:bitrate];
    }
}

-(void)setPacketBufferInfo:(Surge::RtpPacketBufferInfo)packetBufferInfo {
    self.lostPackets = packetBufferInfo.missedPacketsCount;
    self.successfulPacketsReceived = packetBufferInfo.successfulPacketsCount;
    self.outOfOrderPacketsReceived = packetBufferInfo.oooPacketsCount;

    if ([self.diagnosticsDelegate respondsToSelector:@selector(rtspPlayer:didObservePacketLossCount:withSuccessfulPacketsCount:andOutOfOrderPacketsReorderedCount:)]) {
        [self.diagnosticsDelegate rtspPlayer:self.player
                   didObservePacketLossCount:self.lostPackets
                  withSuccessfulPacketsCount:self.successfulPacketsReceived
          andOutOfOrderPacketsReorderedCount:self.outOfOrderPacketsReceived];
    }
}

@end
