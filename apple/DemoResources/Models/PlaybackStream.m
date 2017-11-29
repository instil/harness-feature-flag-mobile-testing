//
//  PlaybackStream.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 10/10/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "PlaybackStream.h"
#import "RtspAddress.h"

@interface PlaybackStream() <SurgeRtspPlayerDelegate>
@property (nonatomic) PlaybackStreamState state;
@end

@implementation PlaybackStream

- (instancetype)initWithRtspAddress:(RtspAddress *)rtspAddress {
  if (self = [super init]) {
    _player = [[SurgeRtspPlayer alloc] init];
    _player.delegate = self;
    _rtspAddress = rtspAddress;
    _state = PlaybackStreamStateIdle;
  }
  return self;
}

- (void)dealloc {
//  [self stop];
  self.player.delegate = nil;
}

#pragma mark - Properties

- (BOOL)isPlaying {
  return self.state == PlaybackStreamStatePlaying;
}

#pragma mark - Transport

- (void)start {
  self.state = PlaybackStreamStateLoading;
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
    NSURL *url = [NSURL URLWithString:self.rtspAddress.address];
    if (self.rtspAddress.username && self.rtspAddress.password) {
      [self.player initiatePlaybackOf:url withUsername:self.rtspAddress.username andPassword:self.rtspAddress.password];
    }
    else {
      [self.player initiatePlaybackOf:url];
    }
  });
}

- (void)play {
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    [self.player play];
  });
}

- (void)stop {
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    [self.player stop];
  });
}

- (void)pause {
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    [self.player pause];
  });
}

#pragma mark - SurgeRtspPlayerDelegate

- (void)rtspPlayerDidBeginPlayback:(SurgeRtspPlayer *)player {
  self.state = PlaybackStreamStatePlaying;
}

- (void)rtspPlayerFailedToInitiatePlayback:(nonnull SurgeRtspPlayer *)player {
  self.state = PlaybackStreamStateErrored;
}

- (void)rtspPlayerDidStopPlayback:(SurgeRtspPlayer *)player {
  self.state = PlaybackStreamStateIdle;
}

- (void)rtspPlayerDidBeginBuffering:(SurgeRtspPlayer *)player {
  self.state = PlaybackStreamStateLoading;
}

- (void)rtspPlayerDidStopBuffering:(SurgeRtspPlayer *)player {
  self.state = PlaybackStreamStatePlaying;
}

- (void)rtspPlayerDidTimeout:(SurgeRtspPlayer *)player {
  
}

- (void)rtspPlayer:(SurgeRtspPlayer *)player didObservePlaybackFrameRate:(NSUInteger)frameRate {
  //  NSLog(@"Frame rate: %@", @(frameRate));
}

@end
