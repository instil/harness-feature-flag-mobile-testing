// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <vector>
#include <string>

namespace SurgeUtil {

    int SetNonBlocking(const int fd);

    std::vector<std::string> StringSplit(const std::string& text, char sep);

    std::vector<std::string> StringSplit(const std::string& data, std::string token);

    std::string FindKeyAndValuePair(const std::vector<std::string>* values, const std::string& key);

    int RandomEvenNumberBetween(int min, int max);

    std::string IntToFixedLengthString(const unsigned int input, const unsigned int length);

};

#endif //__HELPERS_H__
