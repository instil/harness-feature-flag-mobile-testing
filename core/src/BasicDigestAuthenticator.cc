//
//  BasicDigestAuthenticator.cc
//  SurgeCore
//
//  Created by Paul Shields on 26/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "BasicDigestAuthenticator.h"
#include "Base64.h"

std::vector<std::string>* Surge::BasicDigestAuthenticator::OnConnect(const std::string &username, const std::string &password) {
    this->username = username;
    this->password = password;

    std::string auth = username + ":" + password;

    std::vector<std::string> *result = new std::vector<std::string>;
    result->push_back("Authorization: Basic " + SurgeUtil::Base64Encode((const unsigned char *)auth.c_str(), auth.length()));

    return result;
}

std::vector<std::string> Surge::BasicDigestAuthenticator::UnauthorizedError(const Response *response) {
    return std::vector<std::string>();
}
