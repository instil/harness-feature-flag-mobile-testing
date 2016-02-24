// -*-c++-*-
#ifndef __BASE64_H__
#define __BASE64_H__

// Standard C++ Headers
#include <string>

namespace SurgeUtil {
    std::string base64Encoded(const std::string& plain);
    std::string base64Decoded(const std::string& coded);
}

#endif
