//
//  Authenticator.h
//  SurgeFrameworks
//
//  Created by Paul Shields on 30/07/2018.
//

#import <Foundation/Foundation.h>

@protocol Authenticator

@optional
-(NSArray*) authenticationHeadersForUsername:(NSString*)username andPassword:(NSString*)password;
-(NSData*) firstBytesOnTheWireAuthenticationWithUsername:(NSString*)username andPassword:(NSString*)password;

@end
