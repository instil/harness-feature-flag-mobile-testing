//
//  BasicDigestAuthenticator.cc
//  SurgeCore
//
//  Created by Paul Shields on 26/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "BasicDigestAuthenticator.h"
#include "Base64.h"
#include "RegexUtils.h"

Surge::BasicDigestAuthenticator::BasicDigestAuthenticator() : nonce(), realm(), algorithmString(), qopString() {
    Reset();
}

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

    auto authHeader = "Authorization: Digest username=\"" + username + "\", " +
                        "realm=\"" + realm + "\", " +
                        "nonce=\"" + nonce + "\", " +
                        "uri=\"" + url + "\", ";

    hashOne = DigestHashOneFrom(username, password, algorithm, qop);
    hashTwo = DigestHashTwoFrom(method, url, qop);
    responseHash = DigestResponseHashFrom(hashOne, hashTwo, qop);

    if (qop == AUTH || qop == AUTH_INT) {
        authHeader += "qop=\"" + qopString + "\", " +
                        "nc=" + SurgeUtil::IntToFixedLengthString(nonceCount, NC_STRING_LENGTH) + ", " +
                        "cnonce=\"" + cnonce + "\", ";
    }

    authHeader += "response=\"" + responseHash + "\"";

    result.push_back(authHeader);
    return result;
}

std::string Surge::BasicDigestAuthenticator::DigestHashOneFrom(const std::string &username,
                                const std::string &password,
                                const DigestAlgorithm &algorithm,
                                const DigestQop &qop) {
    std::string hashOne = SurgeUtil::Md5::Generate(username + ":" + realm + ":" + password);

    if (algorithm == MD5SESS) {
        hashOne = SurgeUtil::Md5::Generate(hashOne + ":" + nonce + ":" + cnonce);
    }

    return hashOne;
}

std::string Surge::BasicDigestAuthenticator::DigestHashTwoFrom(const std::string &method,
                                                               const std::string &url,
                                                               const DigestQop &qop) {
    switch (qop) {
        case AUTH_INT:
            return SurgeUtil::Md5::Generate(method + ":" + url + ":" + SurgeUtil::Md5::Generate(""));
        case AUTH:
        default:
            return SurgeUtil::Md5::Generate(method + ":" + url);
    }
}
std::string Surge::BasicDigestAuthenticator::DigestResponseHashFrom(const std::string &hashOne,
                                                                    const std::string &hashTwo,
                                                                    const DigestQop &qop) {
    switch (qop) {
        case AUTH:
        case AUTH_INT:
            return SurgeUtil::Md5::Generate(hashOne + ":" +
                                            nonce + ":" +
                                            SurgeUtil::IntToFixedLengthString(nonceCount, NC_STRING_LENGTH) + ":" +
                                            cnonce + ":" +
                                            qopString + ":" +
                                            hashTwo);
        default:
            return SurgeUtil::Md5::Generate(hashOne + ":" + nonce + ":" + hashTwo);
    }
}

bool Surge::BasicDigestAuthenticator::UpdateAuthForUnauthorizedError(const RtspResponse *response) {
    if (response->GetHeaders().find("WWW-Authenticate") != response->GetHeaders().end()) {
        INFO("Using Digest auth");
        usingDigestAuth = true;

        auto authentiateHeader = response->GetHeaders().at("WWW-Authenticate");
        auto nonceHeader = SurgeUtil::RegexUtils::Split(authentiateHeader, "nonce=\"?(.[^ \"]+)\"{1,}");
        auto realmHeader = SurgeUtil::RegexUtils::Split(authentiateHeader, "realm=\"([^\"]*)\"?");
        auto algorithmHeader = SurgeUtil::RegexUtils::Split(authentiateHeader, "algorithm=\"?([A-Za-z0-9]+)\"?");
        auto qopHeader = SurgeUtil::RegexUtils::Split(authentiateHeader, "qop=\"?([A-Za-z0-9]+)\"?");

        nonce = nonceHeader[1];
        realm = realmHeader[1];
        algorithmString = "";
        qopString = "";
        ++nonceCount;

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
