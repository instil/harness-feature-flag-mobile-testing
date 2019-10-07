// Copyright (c) 2019 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "StringUtils.h"

std::string SurgeUtil::String::ToLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string SurgeUtil::String::ToUpperCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

unsigned long SurgeUtil::String::Find(const std::string& haystack, const std::string &needle) {
    return ToLowerCase(haystack).find(ToLowerCase(needle));
}

bool SurgeUtil::String::EndsWith(const std::string& input, const std::string &ending) {
    if (ending.size() > input.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), input.rbegin(), CaseInsensitiveStringEqualsPredicate());
}

bool SurgeUtil::String::StartsWith(const std::string& input, const std::string &ending) {
    if (ending.size() > input.size()) return false;
    return std::equal(ending.begin(), ending.end(), input.begin(), CaseInsensitiveStringEqualsPredicate());
}

bool SurgeUtil::String::Equals(const std::string &firstString, const std::string &secondString) {
    return std::equal(firstString.begin(), firstString.end(), secondString.begin(), secondString.end(), CaseInsensitiveStringEqualsPredicate());
}

bool SurgeUtil::String::Contains(const std::string& haystack, const std::string &needle) {
    return haystack.find(needle) != std::string::npos;
}
