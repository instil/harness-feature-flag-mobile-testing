// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>
#import "SurgeRtspPlayer.h"
#import "SessionDescription.h"

#import "Surge.h"

@interface SurgeRtspPlayer ()

/**
 * Closes and cleans the Surge library, able to play another RTSP stream
 */
- (void)closeStream;

/**
 * Send a DESCRIBE mesage to the RTSP stream
 */
- (void)describe:(void (^)(std::vector<Surge::SessionDescription>, RtspErrorCode))callback;
                  
/**
 * Send a SETUP mesage to the RTSP stream
 */
- (void)setupStream:(Surge::SessionDescription)sessionDescription withCallback:(void (^)(RtspErrorCode))callback;

/**
 * Send a TEARDOWN message to the RTSP stream
 */
- (void)teardown:(void (^)(RtspErrorCode)) callback;

/**
 * Select the preferred initial Session Description, returned from the last DESCRIBE message
 */
- (Surge::SessionDescription)selectPreferredSessionDescription;

/**
 * Set a time range for the stream to play between
 */
- (void)setRangeWithStartTime:(NSDate *)startTime
                   andEndTime:(NSDate *)endTime;


/**
 * RTSP Stream URL
 */
@property (nonatomic, strong) NSURL *url;

/**
 * Optional: Username for the RTSP stream
 */
@property (nonatomic, copy) NSString *username;

/**
 * Optional: Password for the RTSP stream
 */
@property (nonatomic, copy) NSString *password;

/**
 * Time the stream has started playing from. Nil if stream is live
 */
@property (nonatomic, strong) NSDate *startTime;

/**
 * Time the stream will finish playing at. Nil if stream is live
 */
@property (nonatomic, strong) NSDate *endTime;

/**
 * List containing the SessionDescriptions available for this stream
 */
@property (nonatomic, assign) std::vector<Surge::SessionDescription> sessionDescriptions;

/**
 * Instance of the C++ client handling the RTSP connection to the stream
 */
@property (nonatomic, assign) Surge::RtspClient *client;

@end
