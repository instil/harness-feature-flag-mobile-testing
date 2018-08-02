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
#include "Response.h"

namespace Surge {
    class BaseAuthenticator {
    public:
        virtual ~BaseAuthenticator() { }

        virtual std::vector<std::string> AuthenticationHeaders(const std::string &username, const std::string &password) {
            return std::vector<std::string>();
        };
        
        virtual std::vector<char> FirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password) {
            return std::vector<char>();
        };

        virtual std::vector<std::string> UnauthorizedError(const Response *response) {
            return std::vector<std::string>();
        };
    };
}

#endif /* BaseAuthentication_h */
