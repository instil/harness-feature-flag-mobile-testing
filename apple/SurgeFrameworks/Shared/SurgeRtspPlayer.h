// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
 * Called when the player begins or resumes playback of a stream.
 */
- (void)rtspPlayerDidBeginPlayback:(SurgeRtspPlayer *)player;

/**
 * Called when the player enters the buffering state.
 */
- (void)rtspPlayerDidBeginBuffering:(SurgeRtspPlayer *)player;

/**
 * Called when the player exits the buffering state.
 */
- (void)rtspPlayerDidStopBuffering:(SurgeRtspPlayer *)player;

/**
 * Called when the player times out.
 */
- (void)rtspPlayerDidTimeout:(SurgeRtspPlayer *)player;

/**
 * Guaranteed to be call at most once per second with the current player frame rate.
 */
- (void)rtspPlayer:(SurgeRtspPlayer *)player didObservePlaybackFrameRate:(NSUInteger)frameRate;

@end


@interface SurgeRtspPlayer : NSObject

/**
 * Initiate playback of the RTSP stream - this will issue the required
 * describe, setup and play RTSP requests.
 */
- (BOOL)initiatePlaybackOf:(NSURL*)url;


/**
 * Initiate playback of a basic auth protected RTSP stream -
 * this will issue the required describe, setup and play RTSP requests.
 */
- (BOOL)initiatePlaybackOf:(NSURL*)url withUsername:(NSString*)username andPassword:(NSString*)password;

/**
 * Initiate playback of a basic auth protected RTSP stream at a specific start and end time -
 * this will issue the required describe, setup and play RTSP requests.
 */
- (BOOL)initiatePlaybackOf:(NSURL*)url
              withUsername:(NSString *)username
               andPassword:(NSString *)password
                startingAt:(NSDate *)startDate
               andEndingAt:(NSDate *)endDate;

/**
 * Seek to a specific time in the stream.
 */
- (void)seekToStartTime:(NSDate *)startTime
             andEndTime:(NSDate *)endTime;

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
@property (nonatomic, strong, readonly) UIImageView *playerView;
#else
@property (nonatomic, strong, readonly) NSImageView *playerView;
#endif

/**
 * Optional delegate used to receive notification of player events.
 */
@property (nonatomic, weak) id<SurgeRtspPlayerDelegate> delegate;

/*
 * Recorded FPS of the current stream.
 */
@property (nonatomic, readonly) int framesPerSecond;

@end
