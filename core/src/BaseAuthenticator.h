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
#include <vector>

#include "ITransportInterface.h"
#include "RtspResponse.h"

namespace Surge {
    class BaseAuthenticator {
    public:
        virtual ~BaseAuthenticator() { }

        virtual std::vector<std::string> GenerateAuthHeadersFor(const std::string &url, const std::string &method, const std::string &username, const std::string &password) {
            return std::vector<std::string>();
        };
        
        virtual std::vector<char> FirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password) {
            return std::vector<char>();
        };

        virtual bool UpdateAuthForUnauthorizedError(const RtspResponse *response) {
            return false;
        };
    };
}

#endif /* BaseAuthentication_h */
