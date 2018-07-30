//
//  AuthenticatorMapper.m
//  SurgeiOS
//
//  Created by Paul Shields on 30/07/2018.
//

#import "AuthenticatorMapper.h"

std::vector<std::string>* ObjectiveCAuthenticator::OnConnect(const std::string &username, const std::string &password) {
    NSArray *headers = [authenticator onConnectWithUsername:[NSString stringWithCString:username.c_str() encoding:[NSString defaultCStringEncoding]]
                             andPassword:[NSString stringWithCString:password.c_str() encoding:[NSString defaultCStringEncoding]]];

    auto result = new std::vector<std::string>();

    [headers enumerateObjectsUsingBlock:^( NSString * _Nonnull header, NSUInteger index,  BOOL * _Nonnull stop) {
        result->push_back(std::string([header UTF8String]));
    }];

    return result;
}

@implementation AuthenticatorMapper

+(ObjectiveCAuthenticator*) toCoreObject:(id<Authenticator>) authenticator {
    return new ObjectiveCAuthenticator(authenticator);
}

@end
