//
//  BasicDigestAuthenticator.h
//  SurgeCore
//
//  Created by Paul Shields on 26/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef BasicDigestAuthenticator_h
#define BasicDigestAuthenticator_h

#include <stdio.h>

#include "BaseAuthenticator.h"

namespace Surge {
    class BasicDigestAuthenticator : public BaseAuthenticator {
    public:
        BasicDigestAuthenticator() { }
        ~BasicDigestAuthenticator() { }

        std::vector<std::string> AuthenticationHeaders(const std::string &username, const std::string &password) override;
        std::vector<std::string> UnauthorizedError(const Response *response) override;

    private:
        std::string username;
        std::string password;
    };
}

#endif /* BasicDigestAuthenticator_hpp */
