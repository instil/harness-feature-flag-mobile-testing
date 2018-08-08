//
//  AuthenticatorMapper.m
//  SurgeiOS
//
//  Created by Paul Shields on 30/07/2018.
//

#import "AuthenticatorMapper.h"

std::vector<std::string> ObjectiveCAuthenticator::AuthenticationHeaders(const std::string &username, const std::string &password) {
    NSString *user = [NSString stringWithCString:username.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *pass = [NSString stringWithCString:password.c_str() encoding:[NSString defaultCStringEncoding]];

    std::vector<std::string> result;

    if ([authenticator respondsToSelector:@selector(authenticationHeadersForUsername:andPassword:)]) {
        NSArray *headers = [authenticator authenticationHeadersForUsername:user
                                                               andPassword:pass];

        for (id header in headers) {
            result.push_back(std::string([header UTF8String]));
        }
    }

    return result;
}

std::vector<char> ObjectiveCAuthenticator::FirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password) {
    NSString *user = [NSString stringWithCString:username.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *pass = [NSString stringWithCString:password.c_str() encoding:[NSString defaultCStringEncoding]];

    std::vector<char> result;

    if ([authenticator respondsToSelector:@selector(firstBytesOnTheWireAuthenticationWithUsername:andPassword:)]) {
        NSData *bytesToSend = [authenticator firstBytesOnTheWireAuthenticationWithUsername:user
                                                                               andPassword:pass];

        auto size = [bytesToSend length];
        result.resize(size);

        if (size > 0) {
            auto data = (char *)[bytesToSend bytes];
            copy(data, data + size, result.begin());
        }
    }

    return result;
}

@implementation AuthenticatorMapper

+(Surge::BaseAuthenticator*) toCoreObject:(id<SurgeAuthenticator>) authenticator {
    return new ObjectiveCAuthenticator(authenticator);
}

@end
