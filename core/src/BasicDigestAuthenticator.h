//
//  BasicDigestAuthenticator.h
//  SurgeCore
//
//  Created by Paul Shields on 26/07/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef BasicDigestAuthenticator_h
#define BasicDigestAuthenticator_h

#include <stdio.h>

#include "BaseAuthenticator.h"
#include "Helpers.h"

#define DEFAULT_DIGEST_ALGORITHM MD5
#define DEFAULT_DIGEST_QOP DEFAULT
#define NC_STRING_LENGTH 8

namespace Surge {
    class BasicDigestAuthenticator : public BaseAuthenticator {
    private:
        typedef enum {
            MD5,
            MD5SESS,
        } DigestAlgorithm;

        typedef enum {
            DEFAULT,
            AUTH,
            AUTH_INT
        } DigestQop;

    public:
        BasicDigestAuthenticator();
        ~BasicDigestAuthenticator() { }

        std::vector<std::string> GenerateAuthHeadersFor(const std::string &url, const std::string &method, const std::string &username, const std::string &password) override;
        bool UpdateAuthForUnauthorizedError(const RtspResponse *response) override;

    private:
        std::vector<std::string> BasicAuthHeaderFrom(const std::string &username, const std::string &password);
        std::vector<std::string> DigestAuthHeaderFrom(const std::string &url, const std::string &method, const std::string &username, const std::string &password);

    private:
        DigestAlgorithm DigestAlgorithmFromString(std::string value) {
            if (SurgeUtil::StringEqualsInsensitive(value, "MD5-Sess")) {
                return MD5SESS;
            }

            // Algorithm == MD5, or empty default
            return DEFAULT_DIGEST_ALGORITHM;
        }
        DigestQop DigestQopFromString(std::string value) {
            if (SurgeUtil::StringEqualsInsensitive(value, "auth")) {
                return AUTH;
            } else if (SurgeUtil::StringEqualsInsensitive(value, "auth-int")) {
                return AUTH_INT;
            }

            return DEFAULT_DIGEST_QOP;
        }

    private:
        bool usingDigestAuth;
        std::string nonce;
        std::string cnonce;
        int nonceCount;
        std::string realm;
        std::string algorithmString;
        std::string qopString;
    };
}

#endif /* BasicDigestAuthenticator_hpp */
