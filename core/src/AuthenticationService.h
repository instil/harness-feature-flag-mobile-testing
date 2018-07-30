//
//  AuthenticationService.h
//  SurgeCore
//
//  Created by Paul Shields on 27/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef AuthenticationService_h
#define AuthenticationService_h

#include <stdio.h>
#include <vector>

#include "BaseAuthenticator.h"
#include "Response.h"

namespace Surge {
    class AuthenticationService {
    public:
        AuthenticationService();
        ~AuthenticationService();

        void Add(BaseAuthenticator *authenticator) {
            authenticators.push_back(authenticator);
        }

        void Remove(BaseAuthenticator *authenticator) {
            auto test = std::distance(authenticators.begin(), std::find(authenticators.begin(), authenticators.end(), authenticator));
            Remove((unsigned int)test);
        }

        void Remove(unsigned int index) {
            authenticators.erase(authenticators.begin() + index);
        }

        std::string& AuthHeaders() {
            return currentAuthHeaders;
        }
        
        void OnConnect(const std::string &username, const std::string &password);
        bool UnauthorizedError(const Response *response);

    private:
        std::vector<BaseAuthenticator*> authenticators;

        std::string currentAuthHeaders;
    };
}


#endif /* AuthenticationService_h */
