//
//  BaseAuthentication.h
//  SurgeCore
//
//  Created by Paul Shields on 26/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef BaseAuthentication_h
#define BaseAuthentication_h

#include <stdio.h>

#include "ITransportInterface.h"
#include "Response.h"

namespace Surge {
    class BaseAuthenticator {
    public:
        BaseAuthenticator() : transport(nullptr) { }
        virtual ~BaseAuthenticator() { }

        virtual std::vector<std::string>* OnConnect(const std::string &username, const std::string &password) = 0;
        virtual std::vector<std::string> UnauthorizedError(const Response *response) {
            return std::vector<std::string>();
        };

        void SetTransport(ITransportInterface *transport) {
            this->transport = transport;
        }

    protected:
        void SendDataThroughTransport(const char *data, size_t size) {
            if (transport != nullptr) {
                transport->ArbitraryDataTransaction(data, size);
            }
        }

    private:
        ITransportInterface *transport;
    };
}

#endif /* BaseAuthentication_h */
