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
  [self stop];
  self.player.delegate = nil;
}

- (void)start {
  self.state = PlaybackStreamStateLoading;
  __weak typeof(self) weakSelf = self;
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
    NSURL *url = [NSURL URLWithString:weakSelf.rtspAddress.address];
    if (weakSelf.rtspAddress.username && weakSelf.rtspAddress.password) {
      [weakSelf.player initiatePlaybackOf:url withUsername:weakSelf.rtspAddress.username andPassword:weakSelf.rtspAddress.password];
    }
    else {
      [weakSelf.player initiatePlaybackOf:url];
    }
  });
}

- (void)play {
  __weak typeof(self) weakSelf = self;
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    [weakSelf.player play];
  });
}

- (void)stop {
  __weak typeof(self) weakSelf = self;
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    [weakSelf.player stop];
  });
}

- (void)pause {
  __weak typeof(self) weakSelf = self;
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    [weakSelf.player pause];
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
