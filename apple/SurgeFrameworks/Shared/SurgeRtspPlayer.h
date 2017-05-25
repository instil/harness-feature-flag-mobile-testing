// Copyright (c) 2017 Instil Software.
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

@protocol SurgeRtspPlayerDelegate <NSObject>

@optional

/**
 *
 */
- (void)rtspPlayerInitiatedPlayback:(nonnull SurgeRtspPlayer *)player;

/**
 *
 */
- (void)rtspPlayerFailedToInitiatePlayback:(nonnull SurgeRtspPlayer *)player;

/**
 * Called when the player begins or resumes playback of a stream.
 */
- (void)rtspPlayerDidBeginPlayback:(nonnull SurgeRtspPlayer *)player;

/**
 * Called when the player stops or pauses playback of a stream.
 */
- (void)rtspPlayerDidStopPlayback:(nonnull SurgeRtspPlayer *)player;

/**
 * Called when the player enters the buffering state.
 */
- (void)rtspPlayerDidBeginBuffering:(nonnull SurgeRtspPlayer *)player;

/**
 * Called when the player exits the buffering state.
 */
- (void)rtspPlayerDidStopBuffering:(nonnull SurgeRtspPlayer *)player;

/**
 * Called when the player times out.
 */
- (void)rtspPlayerDidTimeout:(nonnull SurgeRtspPlayer *)player;

/**
 * Guaranteed to be call at most once per second with the current player frame rate.
 */
- (void)rtspPlayer:(nonnull SurgeRtspPlayer *)player didObservePlaybackFrameRate:(NSUInteger)frameRate;

@end


@interface SurgeRtspPlayer : NSObject

/**
 * Initiate playback of the RTSP stream - this will issue the required
 * describe, setup and play RTSP requests.
 */
- (void)initiatePlaybackOf:(nonnull NSURL *)url;


/**
 * Initiate playback of a basic auth protected RTSP stream -
 * this will issue the required describe, setup and play RTSP requests.
 */
- (void)initiatePlaybackOf:(nonnull NSURL *)url withUsername:(nonnull NSString *)username andPassword:(nonnull NSString *)password;

/**
 * Initiate playback of a basic auth protected RTSP stream at a specific start and end time -
 * this will issue the required describe, setup and play RTSP requests.
 */
- (void)initiatePlaybackOf:(nonnull NSURL *)url
              withUsername:(nonnull NSString *)username
               andPassword:(nonnull NSString *)password
                startingAt:(nullable NSDate *)startDate
               andEndingAt:(nullable NSDate *)endDate;

/**
 * Seek to a specific time in the stream.
 */
- (void)seekToStartTime:(nullable NSDate *)startTime
             andEndTime:(nullable NSDate *)endTime;

/**
 * Begin/resume playback of the stream.
 */
- (void)play;

/**
 * Pause playback of the stream.
 */
- (void)pause;

/**
 * Stop playback of the stream.
 */
- (void)stop;

/**
 * The basic player view without controls.
 */
#if TARGET_OS_IPHONE
@property (nonatomic, strong, nonnull) UIImageView *playerView;
#else
@property (nonatomic, strong, nonnull) NSImageView *playerView;
#endif

/**
 * Optional delegate used to receive notification of player events.
 */
@property (nonatomic, strong, nullable) id<SurgeRtspPlayerDelegate> delegate;

/*
 * Recorded FPS of the current stream.
 */
@property (nonatomic, readonly) int framesPerSecond;

@end
