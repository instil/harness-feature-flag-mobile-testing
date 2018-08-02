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
#include "ITransportInterface.h"

namespace Surge {
    class AuthenticationService {
    public:
        AuthenticationService();
        ~AuthenticationService();

        void GenerateAuthHeaders(const std::string &username, const std::string &password);
        void ExecuteFirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password);

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


    private:
        std::vector<BaseAuthenticator*> authenticators;
        ITransportInterface *transport;

        std::string currentAuthHeaders;
    };
}


#endif /* AuthenticationService_h */
