// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __BASE64_H__
#define __BASE64_H__

// Standard C++ Headers
#include <string>

namespace SurgeUtil {
    
    std::string Base64Encode(const unsigned char * bytes_to_encode, unsigned int in_len);

    std::string Base64Decode(const std::string& plain);
}

#endif
