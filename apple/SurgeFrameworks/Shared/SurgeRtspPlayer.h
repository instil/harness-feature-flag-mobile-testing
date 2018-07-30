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

#import "Authenticator.h"

@class SurgeRtspPlayer;

typedef NS_ENUM(NSInteger, RtspErrorCode) {
    RtspErrorCodeUnknownFailure = -1,
    RtspErrorCodeSuccess = 200,
    RtspErrorCodeRedirect = 302,
    RtspErrorCodeMethodNotAllowed = 405,
    RtspErrorCodeParameterNotUnderstood = 451,
    RtspErrorCodeConferenceNotFound = 452,
    RtspErrorCodeNotEnoughBandwidth = 453,
    RtspErrorCodeSessionNotFound = 454,
    RtspErrorCodeMethodNotValidInThisState = 455,
    RtspErrorCodeHeaderFieldNotValidInThisState = 456,
    RtspErrorCodeInvalidRange = 457,
    RtspErrorCodeParameterIsReadOnly = 458,
    RtspErrorCodeAggregateOperationNotAllowed = 459,
    RtspErrorCodeOnlyAggregationOperationAllowed = 460,
    RtspErrorCodeUnsupportedTransport = 461,
    RtspErrorCodeDestinationUnreachable = 462,
    RtspErrorCodeOptionNotSupported = 551
};

@protocol SurgeRtspPlayerDelegate <NSObject>

@optional

/**
 * Called when the player successfully starts playback of a stream via an initiatePlaybackOf request.
 */
- (void)rtspPlayerInitiatedPlayback:(nonnull SurgeRtspPlayer *)player;

/**
 * Called when the player fails to start playback of a stream via an initiatePlaybackOf request.
 */
- (void)rtspPlayerFailedToInitiatePlayback:(nonnull SurgeRtspPlayer *)player withErrorCode: (RtspErrorCode)errorCode;

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
@property (nonatomic, weak, nullable) id<SurgeRtspPlayerDelegate> delegate;

/**
 * Maximum length of time, in milliseconds, between frames before Surge will issue a timeout signal
 */
@property (nonatomic, assign) int timeout;

/*
 * Recorded FPS of the current stream.
 */
@property (nonatomic, readonly) int framesPerSecond;

/*
 * If true, Surge will stream video data via an interleaved TCP transport rather than via UDP
 */
@property (nonatomic, assign) bool interleavedTcpTransport;

/*
 * Time delay, in milliseconds, between Surge receiving a packet containing video data and the frame data being decoded for viewing. Used for error correction and packet reordering purposes for the UDP transport. Default: 200ms
 */
@property (nonatomic, assign) int packetBufferDelay;

/*
 * Enables/disables TLS certificate validation when streaming from a TLS protected RTSP stream. Default: true
 */
@property (nonatomic, assign) bool tlsCertificateVerificationEnabled;

/*
 * Allows/disallows self signed TLS certificates from being used when streaming from a TLS protected RTSP stream. Default: true
 */
@property (nonatomic, assign) bool tlsSelfSignedCertificateAllowed;

/*
 * Optional: The path to a trusted root certificate used to validate a TLS certificate received through the TLS handshaking process.
 */
@property (nonatomic, strong, nullable) NSURL *tlsTrustedCertificatePath;

/*
 * Optional: Custom RTSP authentication logic, can be used if a provided RTSP stream is protected by a bespoke non-standard authentication method.
 */
@property (nonatomic, strong, nullable) id<Authenticator> authenticator;

@end
