//
//  PlaybackManager.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 10/10/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SurgeiOS/SurgeiOS.h>

extern NSString *const PlaybackStreamsDidChangeNotification;

@class PlaybackStream;
@class RtspAddress;

@interface PlaybackManager : NSObject
@property (copy, readonly, nonatomic) NSArray<PlaybackStream *> *streams;
- (void)startStreamWithAddress:(RtspAddress *)address;
- (void)removeStream:(PlaybackStream *)stream;
- (void)removeStreamAtIndex:(NSUInteger)idx;
@end
