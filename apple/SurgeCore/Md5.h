//
//  Md5.h
//  SurgeCore
//
//  Created by Paul Shields on 07/09/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef Md5_h
#define Md5_h

#import "Md5Algorithm.h"

namespace SurgeUtil {
    namespace Md5 {
        static std::string Generate(const std::string &source) {
            MD5_CTX md5handler;
            unsigned char md5digest[MD5_DIGEST_LENGTH];
            MD5_Init(&md5handler);
            MD5_Update(&md5handler, source.c_str(), source.length());
            MD5_Final(md5digest, &md5handler);
            std::string hash;
            for (std::size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
                char hex[3];
                snprintf(hex, sizeof (hex), "%02x", md5digest[i]);
                hash.append(hex);
            }
            return hash;
        }
    }
}

#endif /* NotMd5_h */
