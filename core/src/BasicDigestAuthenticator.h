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
#include "Md5.h"
#include "DateTime.h"

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

        void Reset() override {
            usingDigestAuth = false;
            nonceCount = 0;
            cnonce = GenerateNonce();
        }

    private:
        std::vector<std::string> BasicAuthHeaderFrom(const std::string &username, const std::string &password);
        std::vector<std::string> DigestAuthHeaderFrom(const std::string &url, const std::string &method, const std::string &username, const std::string &password);

        std::string DigestHashOneFrom(const std::string &username,
                                      const std::string &password,
                                      const DigestAlgorithm &algorithm,
                                      const DigestQop &qop);
        std::string DigestHashTwoFrom(const std::string &method,
                                      const std::string &url,
                                      const DigestQop &qop);
        std::string DigestResponseHashFrom(const std::string &hashOne,
                                           const std::string &hashTwo,
                                           const DigestQop &qop);

    private:
        DigestAlgorithm DigestAlgorithmFromString(std::string value) {
            if (SurgeUtil::CaseInsensitiveStringEquals(value, "MD5-Sess")) {
                return MD5SESS;
            }

            // Algorithm == MD5, or empty default
            return DEFAULT_DIGEST_ALGORITHM;
        }

        DigestQop DigestQopFromString(std::string value) {
            if (SurgeUtil::CaseInsensitiveStringEquals(value, "auth")) {
                return AUTH;
            } else if (SurgeUtil::CaseInsensitiveStringEquals(value, "auth-int")) {
                return AUTH_INT;
            }

            return DEFAULT_DIGEST_QOP;
        }

    private:
        std::string GenerateNonce() {
            return SurgeUtil::Md5::Generate(SurgeUtil::DateTime::CurrentTime().ConvertToISO());
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
