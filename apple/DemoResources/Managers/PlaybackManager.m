//
//  PlaybackManager.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 10/10/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "PlaybackManager.h"
#import "PlaybackStream.h"
#import "RtspAddress.h"

NSString *const PlaybackStreamsDidChangeNotification = @"PlaybackStreamsDidChangeNotification";

@interface PlaybackManager()
@property (strong, nonatomic) NSMutableArray<PlaybackStream *> *streams;
@end

@implementation PlaybackManager

- (instancetype)init
{
  self = [super init];
  if (self) {
    _streams = [[NSMutableArray<PlaybackStream *> alloc] initWithCapacity:0];
  }
  return self;
}

- (void)startStreamWithAddress:(RtspAddress *)address {
  NSUInteger idx = [self.streams indexOfObjectPassingTest:^BOOL(PlaybackStream * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
    return [obj.rtspAddress.address isEqualToString:address.address];
  }];
  if (idx < self.streams.count) return;
  PlaybackStream *stream = [[PlaybackStream alloc] initWithRtspAddress:address];
  [stream start];
  [_streams addObject:stream];
  [[NSNotificationCenter defaultCenter] postNotificationName:PlaybackStreamsDidChangeNotification object:nil];
}

- (void)removeStream:(PlaybackStream *)stream {
  [stream stop];
  [_streams removeObject:stream];
  [[NSNotificationCenter defaultCenter] postNotificationName:PlaybackStreamsDidChangeNotification object:nil];
}

- (void)removeStreamAtIndex:(NSUInteger)idx {
  if (idx >= self.streams.count) return;
  [self removeStream:self.streams[idx]];
}

@end
