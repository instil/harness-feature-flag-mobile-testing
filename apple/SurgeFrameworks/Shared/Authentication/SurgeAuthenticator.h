//
//  Authenticator.h
//  SurgeFrameworks
//
//  Created by Paul Shields on 30/07/2018.
//

#import <Foundation/Foundation.h>

@protocol SurgeAuthenticator<NSObject>

@optional

/*
 * Provides Surge with custom headers to append to each RTSP request.
 */
- (NSArray *)authenticationHeadersForUsername:(NSString *)username andPassword:(NSString *)password;

/*
 * Provides a payload to be sent, encrypted (if applicable), down the pipe to authenticate the stream
 * before Surge will attempt to execute any RTSP requests.
 */
- (NSData *)firstBytesOnTheWireAuthenticationWithUsername:(NSString *)username andPassword:(NSString *)password;

@end
