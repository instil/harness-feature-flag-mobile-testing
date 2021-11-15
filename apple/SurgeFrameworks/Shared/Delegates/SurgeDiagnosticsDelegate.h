// Copyright (c) 2018 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <AppKit/AppKit.h>
#endif

@class SurgeRtspPlayer;

@protocol SurgeDiagnosticsDelegate <NSObject>

@optional

/**
 * Called every time the player calculates a new value for the number of frames arrived in the last second.
 */
- (void)rtspPlayer:(nonnull SurgeRtspPlayer *)player didObservePlaybackFrameRate:(NSUInteger)frameRate;

/**
 * Called every time the player calculates the bitrate of the video arriving from in the last second.
 */
- (void)rtspPlayer:(nonnull SurgeRtspPlayer *)player didObservePlaybackBitrate:(int)bitrate;

/**
 * Called once a second by the player with the latest information on the number of packets that have arrived as expected, have been lost or have arrived late.
 */
- (void)rtspPlayer:(nonnull SurgeRtspPlayer *)player didObservePacketLossCount:(long)packetLossCount
                                                    withSuccessfulPacketsCount:(long)successfulPacketCount
                                            andOutOfOrderPacketsReorderedCount:(long)oooPacketsCount;

/**
 * Called by the player any time the frame dimensions of the currently playing stream are changed.
 */
- (void)rtspPlayer:(nonnull SurgeRtspPlayer *)player didChangeMediaSize:(CGSize)mediaSize;

/**
 * Called by the player any time the video format of the currently playing stream are changed.
 */
- (void)rtspPlayer:(nonnull SurgeRtspPlayer *)player didChangeMediaFormat:(int)mediaSize;

@end

