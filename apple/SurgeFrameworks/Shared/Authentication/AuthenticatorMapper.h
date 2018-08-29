//
//  AuthenticatorMapper.h
//  SurgeiOS
//
//  Created by Paul Shields on 30/07/2018.
//

#import "SurgeAuthenticator.h"
#import "BaseAuthenticator.h"

class ObjectiveCAuthenticator : public Surge::BaseAuthenticator {
public:
    ObjectiveCAuthenticator(id<SurgeAuthenticator> authenticator) : authenticator(authenticator) { }

    std::vector<std::string> GenerateAuthHeadersFor(const std::string &url, const std::string &method, const std::string &username, const std::string &password) override;
    std::vector<char> FirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password) override;

private:
    id<SurgeAuthenticator> authenticator;
};

@interface AuthenticatorMapper : NSObject

+(Surge::BaseAuthenticator*) toCoreObject:(id<SurgeAuthenticator>) authenticator;

@end
