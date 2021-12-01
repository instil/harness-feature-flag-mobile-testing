// Copyright (c) 2018 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

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

@end
