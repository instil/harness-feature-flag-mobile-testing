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

    NSArray *headers = [authenticator authenticationHeadersForUsername:user
                                                           andPassword:pass];

    std::vector<std::string> result;
    std::vector<std::string> *resultPointer = &result;

    [headers enumerateObjectsUsingBlock:^( NSString * _Nonnull header, NSUInteger index,  BOOL * _Nonnull stop) {
        resultPointer->push_back(std::string([header UTF8String]));
    }];

    return result;
}

std::vector<char> ObjectiveCAuthenticator::FirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password) {
    NSString *user = [NSString stringWithCString:username.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *pass = [NSString stringWithCString:password.c_str() encoding:[NSString defaultCStringEncoding]];

    NSData *bytesToSend = [authenticator firstBytesOnTheWireAuthenticationWithUsername:user
                                                                           andPassword:pass];

    auto size = [bytesToSend length];
    std::vector<char> result(size);

    if (size > 0) {
        auto data = (char *)[bytesToSend bytes];
        copy(data, data + size, result.begin());
    }

    return result;
}

@implementation AuthenticatorMapper

+(Surge::BaseAuthenticator*) toCoreObject:(id<Authenticator>) authenticator {
    return new ObjectiveCAuthenticator(authenticator);
}

@end
