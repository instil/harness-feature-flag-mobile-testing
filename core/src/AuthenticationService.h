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

        std::string GenerateAuthHeadersFor(const std::string &method);
        void ExecuteFirstBytesOnTheWireAuthentication();
        bool UpdateAuthForUnauthorizedError(const RtspResponse *response);

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

        void SetTransport(ITransportInterface *transport) {
            this->transport = transport;
        }

        void SetStreamCredentials(std::string url, std::string username, std::string password) {
            this->url = url;
            this->username = username;
            this->password = password;

            ForEachAuthenticator([this](auto authenticator) {
                authenticator->Reset();
            });
        }

    private:
        void ForEachAuthenticator(std::function<void(BaseAuthenticator*)> callback) {
            std::for_each(authenticators.begin(),
                          authenticators.end(),
                          callback);
        }


    private:
        std::vector<BaseAuthenticator*> authenticators;
        ITransportInterface *transport;

        std::string url;
        std::string username;
        std::string password;
    };
}


#endif /* AuthenticationService_h */
