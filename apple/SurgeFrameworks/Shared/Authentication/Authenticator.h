//
//  Authenticator.h
//  SurgeFrameworks
//
//  Created by Paul Shields on 30/07/2018.
//

#import <Foundation/Foundation.h>

@protocol Authenticator

-(NSArray*) onConnectWithUsername:(NSString*)username andPassword:(NSString*)password;

@end
