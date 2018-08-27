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
#include "RtspResponse.h"
#include "ITransportInterface.h"

namespace Surge {
    class AuthenticationService {
    public:
        AuthenticationService();
        ~AuthenticationService();

        void GenerateAuthHeaders();
        void ExecuteFirstBytesOnTheWireAuthentication();

        void Add(BaseAuthenticator *authenticator) {
            authenticators.push_back(authenticator);
        }

        void Remove(BaseAuthenticator *authenticator) {
            auto index = (unsigned int)std::distance(authenticators.begin(),
                                                     std::find(authenticators.begin(),
                                                               authenticators.end(),
                                                               authenticator));
            Remove(index);
        }

        void Remove(unsigned int index) {
            authenticators.erase(authenticators.begin() + index);
        }

        std::string& AuthHeaders() {
            return currentAuthHeaders;
        }

        void SetTransport(ITransportInterface *transport) {
            this->transport = transport;
        }

        void SetCredentials(std::string username, std::string password) {
            this->username = username;
            this->password = password;
        }


    private:
        std::vector<BaseAuthenticator*> authenticators;
        ITransportInterface *transport;

        std::string username;
        std::string password;

        std::string currentAuthHeaders;
    };
}


#endif /* AuthenticationService_h */
