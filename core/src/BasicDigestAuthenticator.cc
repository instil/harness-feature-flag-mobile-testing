//
//  BasicDigestAuthenticator.cc
//  SurgeCore
//
//  Created by Paul Shields on 26/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "BasicDigestAuthenticator.h"
#include "Base64.h"
#include "Md5.h"
#include "Regex.h"

Surge::BasicDigestAuthenticator::BasicDigestAuthenticator() : usingDigestAuth(false), nonce(), cnonce("4c2031e933365ee87bd8c378f8aa7c68"), nonceCount(0), realm(), algorithmString(), qopString() { }

std::vector<std::string> Surge::BasicDigestAuthenticator::GenerateAuthHeadersFor(const std::string &url, const std::string &method, const std::string &username, const std::string &password) {
    if (username.length() > 0 && password.length() > 0) {

        if (usingDigestAuth) {
            return DigestAuthHeaderFrom(url, method, username, password);
        }

        return BasicAuthHeaderFrom(username, password);
    }

    return std::vector<std::string>();
}

std::vector<std::string> Surge::BasicDigestAuthenticator::BasicAuthHeaderFrom(const std::string &username, const std::string &password) {
    std::string auth = username + ":" + password;

    std::vector<std::string> result;
    result.push_back("Authorization: Basic " + SurgeUtil::Base64Encode((const unsigned char *)auth.c_str(), auth.length()));

    return result;
}

std::vector<std::string> Surge::BasicDigestAuthenticator::DigestAuthHeaderFrom(const std::string &url, const std::string &method, const std::string &username, const std::string &password) {
    std::vector<std::string> result;

    auto algorithm = DigestAlgorithmFromString(algorithmString);
    auto qop = DigestQopFromString(qopString);

    std::string hashOne;
    std::string hashTwo;
    std::string responseHash;

    auto authHeader = "Authorization: Digest username=\"" + username + "\"" +
                        "realm=\"" + realm + "\"" +
                        "nonce=\"" + nonce + "\"" +
                        "uri=\"" + url + "\"";

    hashOne = SurgeUtil::Md5::Generate(username + ":" + realm + ":" + password);

    if (algorithm == MD5SESS) {
        hashOne = SurgeUtil::Md5::Generate(hashOne + ":" + nonce + ":" + cnonce);
    }

    switch (qop) {
        case AUTH_INT:
            hashTwo = SurgeUtil::Md5::Generate(method + ":" + url + ":" + SurgeUtil::Md5::Generate(""));
            break;
        case AUTH:
        default:
            hashTwo = SurgeUtil::Md5::Generate(method + ":" + url);
            break;
    }

    switch (qop) {
        case AUTH:
        case AUTH_INT:
            responseHash = SurgeUtil::Md5::Generate(hashOne + ":" +
                                                    nonce + ":" +
                                                    SurgeUtil::IntToFixedLengthString(nonceCount, NC_STRING_LENGTH) + ":" +
                                                    cnonce + ":" +
                                                    qopString + ":" +
                                                    hashTwo);

            authHeader += "qop=\"" + qopString + "\", " +
                            "nc=" + SurgeUtil::IntToFixedLengthString(nonceCount, NC_STRING_LENGTH) + ", " +
                            "cnonce=\"" + cnonce + "\", ";
            break;
        default:
            responseHash = SurgeUtil::Md5::Generate(hashOne + ":" + nonce + ":" + hashTwo);
            break;
    }

    authHeader += "response=\"" + responseHash + "\"";

    result.push_back(authHeader);
    return result;
}

bool Surge::BasicDigestAuthenticator::UpdateAuthForUnauthorizedError(const RtspResponse *response) {
    if (response->GetHeaders().find("WWW-Authenticate") != response->GetHeaders().end()) {
        INFO("Using Digest auth");
        usingDigestAuth = true;

        auto authentiateHeader = response->GetHeaders().at("WWW-Authenticate");
        auto nonceHeader = SurgeUtil::Regex::Split(authentiateHeader, "nonce=\"?([A-Za-z0-9]+)\"?");
        auto realmHeader = SurgeUtil::Regex::Split(authentiateHeader, "realm=\"?([A-Za-z0-9 ]+)\"?");
        auto algorithmHeader = SurgeUtil::Regex::Split(authentiateHeader, "algorithm=\"?([A-Za-z0-9]+)\"?");
        auto qopHeader = SurgeUtil::Regex::Split(authentiateHeader, "qop=\"?([A-Za-z0-9]+)\"?");

        nonce = nonceHeader[1];
        realm = realmHeader[1];
        algorithmString = "";
        qopString = "";
        ++nonceCount;   // TODO: Need to reset string length

        if (algorithmHeader.size() >= 2) {
            algorithmString = algorithmHeader[1];
        }

        if (qopHeader.size() >= 2) {
            qopString = qopHeader[1];
        }

        return true;
    }
    
    return false;
}
