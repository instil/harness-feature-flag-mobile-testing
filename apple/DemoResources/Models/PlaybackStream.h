//
//  PlaybackStream.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 10/10/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SurgeiOS/SurgeiOS.h>

typedef NS_ENUM(NSUInteger, PlaybackStreamState) {
  PlaybackStreamStateIdle,
  PlaybackStreamStateLoading,
  PlaybackStreamStatePlaying,
  PlaybackStreamStateErrored
};

@class RtspAddress;

@interface PlaybackStream : NSObject
- (instancetype)initWithRtspAddress:(RtspAddress *)address;
@property (readonly, strong, nonatomic) RtspAddress *rtspAddress;
@property (readonly, strong, nonatomic) SurgeRtspPlayer *player;
@property (readonly, nonatomic) PlaybackStreamState state;
@property (readonly, nonatomic) BOOL isPlaying;
- (void)start;
- (void)play;
- (void)stop;
- (void)pause;
@end
