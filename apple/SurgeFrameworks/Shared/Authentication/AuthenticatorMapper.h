//
//  AuthenticatorMapper.h
//  SurgeiOS
//
//  Created by Paul Shields on 30/07/2018.
//

#import <UIKit/UIKit.h>

#import "Authenticator.h"
#import "BaseAuthenticator.h"

class ObjectiveCAuthenticator : public Surge::BaseAuthenticator {
public:
    ObjectiveCAuthenticator(id<Authenticator> authenticator) : authenticator(authenticator) { }

    std::vector<std::string>* OnConnect(const std::string &username, const std::string &password) override;

private:
    id<Authenticator> authenticator;
};

@interface AuthenticatorMapper : NSObject

+(ObjectiveCAuthenticator*) toCoreObject:(id<Authenticator>) authenticator;

@end
