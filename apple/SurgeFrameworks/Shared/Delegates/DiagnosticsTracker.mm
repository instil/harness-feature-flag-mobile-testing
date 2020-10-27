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

@property (nonatomic, weak) SurgeRtspPlayer *player;

@property (nonatomic, strong) dispatch_queue_t trackerQueue;
@property (nonatomic, assign) BOOL queueHasStarted;

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
        self.queueHasStarted = false;
    }
    return self;
}

- (void)startTracking {
    if (!self.queueHasStarted) {
        SurgeLogDebug(@"Starting diagnostics tracker");
        self.queueHasStarted = true;
        [self updateDelegate];
    } else {
        SurgeLogDebug(@"Restarting diagnostics tracker");
        dispatch_resume(self.trackerQueue);
    }
}

- (void)stopTracking {
    SurgeLogDebug(@"Stopping diagnostics tracker");
    dispatch_suspend(self.trackerQueue);
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

- (void)updateDelegate {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), self.trackerQueue, ^{
        self.framesPerSecond = self.framePerSecondCounter;
        self.framePerSecondCounter = 0;

        self.bitrate = self.bitrateCounter;
        self.bitrateCounter = 0;

        [self setPacketBufferInfo: self.player.client->GetDiagnosticsOnPacketLoss()];

        [self updateDelegate];
    });
}

- (void)setFramesPerSecond:(int)framesPerSecond {
    _framesPerSecond = framesPerSecond;

    if ([self.diagnosticsDelegate respondsToSelector:@selector(rtspPlayer:didObservePlaybackFrameRate:)]) {
        [self.diagnosticsDelegate rtspPlayer:self.player didObservePlaybackFrameRate:self.framesPerSecond];
    }

    if ([self.depreciatedDiagnosticsDelegate respondsToSelector:@selector(rtspPlayer:didObservePlaybackFrameRate:)] &&
        ![self.diagnosticsDelegate isKindOfClass:[self.depreciatedDiagnosticsDelegate class]]) {
        [self.depreciatedDiagnosticsDelegate rtspPlayer:self.player didObservePlaybackFrameRate:self.framesPerSecond];
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
