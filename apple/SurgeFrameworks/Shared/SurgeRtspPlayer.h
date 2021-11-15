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

#import "SurgeAuthenticator.h"
#import "SurgeRtspPlayerDelegate.h"
#import "SurgeDiagnosticsDelegate.h"
#import "SurgeDiagnostics.h"

@class SurgeRtspPlayer;

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
 * Optional delegate used to receive any diagnostics updates as they are calculated by the Diagnostics object.
 */
@property (nonatomic, weak, nullable) id<SurgeDiagnosticsDelegate> diagnosticsDelegate;

/**
 * Maximum length of time, in milliseconds, between frames before Surge will issue a timeout signal.
 */
@property (nonatomic, assign) int timeout;

/*
 * Recorded FPS of the current stream.
 */
@property (nonatomic, readonly) int framesPerSecond DEPRECATED_ATTRIBUTE;

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
@property (nonatomic, assign) BOOL tlsCertificateValidationEnabled;

/*
 * Allows/disallows self signed TLS certificates from being used when streaming from a TLS protected RTSP stream. Default: true
 */
@property (nonatomic, assign) BOOL tlsSelfSignedCertificateAllowed;

/*
 * Optional: The path to a trusted root certificate used to validate a TLS certificate received through the TLS handshaking process.
 */
@property (nonatomic, strong, nullable) NSURL *tlsTrustedCertificatePath;

/*
 * Optional: Custom RTSP authentication logic, can be used if a provided RTSP stream is protected by a bespoke non-standard authentication method.
 */
@property (nonatomic, weak, nullable) id<SurgeAuthenticator> authenticator;

/*
 * Item containing the latest diagnostics information for the currently playing stream.
 */
@property (nonatomic, readonly, nonnull) id<SurgeDiagnostics> diagnostics;

/*
 * Timeout for RTSP requests.
 */
@property (nonatomic, assign) long long int rtspTimeout;

@end
