//
//  BasicDigestAuthenticator.cc
//  SurgeCore
//
//  Created by Paul Shields on 26/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "BasicDigestAuthenticator.h"
#include "Base64.h"

std::vector<std::string> Surge::BasicDigestAuthenticator::AuthenticationHeaders(const std::string &username, const std::string &password) {
    if (username.length() > 0 && password.length() > 0) {
        this->username = username;
        this->password = password;

        std::string auth = username + ":" + password;

        std::vector<std::string> result;
        result.push_back("Authorization: Basic " + SurgeUtil::Base64Encode((const unsigned char *)auth.c_str(), auth.length()));

        return result;
    }

    return std::vector<std::string>();
}

std::vector<std::string> Surge::BasicDigestAuthenticator::UnauthorizedError(const std::string &username, const std::string &password, const RtspResponse *response) {
    std::vector<std::string> result;

    return result;
}
