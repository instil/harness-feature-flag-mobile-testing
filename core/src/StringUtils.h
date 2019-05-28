// Copyright (c) 2019 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef StringUtils_h
#define StringUtils_h

#include <stdio.h>

#include <vector>
#include <string>

namespace SurgeUtil {
    namespace String {
        std::string ToLowerCase(const std::string& input);
        std::string ToUpperCase(const std::string& input);
        unsigned long Find(const std::string& haystack, const std::string &needle);
        bool EndsWith(const std::string& input, const std::string &ending);
        bool StartsWith(const std::string& input, const std::string &ending);
        bool Equals(const std::string &firstString, const std::string &secondString);
        bool Contains(const std::string& haystack, const std::string &needle);

        struct CaseInsensitiveStringEqualsPredicate {
            bool operator()(int c1, int c2) const {
                return std::toupper(c1) == std::toupper(c2);
            }
        };
    }
};

#endif /* StringUtils_h */
