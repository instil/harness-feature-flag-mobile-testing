// Copyright (c) 2018 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

@protocol SurgeDiagnostics<NSObject>

@required

/**
 * Recorded FPS of the current stream.
 */
@property (nonatomic, readonly) int framesPerSecond;

/**
 * Recorded bitrate of the current stream.
 */
@property (nonatomic, readonly) size_t bitrate;

/**
 * Number of packets which have arrived out of order and have had to be reordered before a frame was decoded in the lifetime of the currently playing stream.
 */
@property (nonatomic, readonly) long outOfOrderPacketsReceived;

/**
 * Number of packets which have successfully arrived in order as expected in the lifetime of the currently playing stream.
 */
@property (nonatomic, readonly) long successfulPacketsReceived;

/**
 * Number of packets which have been lost/never arrived in the lifetime of the currently playing stream.
 */
@property (nonatomic, readonly) long lostPackets;

/**
 * Video frame dimensions of the current stream.
 */
@property (nonatomic, readonly) CGSize mediaDimensions;

@end
