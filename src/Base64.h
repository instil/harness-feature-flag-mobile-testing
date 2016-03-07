// -*-c++-*-
#ifndef __BASE64_H__
#define __BASE64_H__

// Standard C++ Headers
#include <string>

namespace SurgeUtil {
    
    std::string Base64Encode(const unsigned char * bytes_to_encode, unsigned int in_len);

    std::string Base64Decode(const std::string& plain);
}

#endif
