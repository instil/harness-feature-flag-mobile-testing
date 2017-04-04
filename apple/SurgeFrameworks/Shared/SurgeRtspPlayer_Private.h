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
- (void)describe:(void (^)(void)) callback;

/**
 * Send a SETUP mesage to the RTSP stream
 */
- (void)setupStream:(Surge::SessionDescription)sessionDescription withCallback:(void (^)(void))callback;

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
