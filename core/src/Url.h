// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __URL_H__
#define __URL_H__

#include <string>
#include <map>
#include <stdexcept>

#include "Helpers.h"

#include "RegexUtils.h"
#include "Logging.h"

#include "Helpers.h"
#include "StringUtils.h"

#define URL_MATCH_REGEX "(rtsps?)://([A-Za-z0-9.]+|[[A-Za-z0-9:]+]):?([0-9]+)?/?([A-Za-z0-9-_/]+)?(\\?[A-Za-z0-9-_/=&]+)?"
#define RTSP_PROTOCOL "rtsp"
#define SECURE_RTSP_PROTOCOL "rtsps"
#define DEFAULT_RTSP_PORT "554"


namespace SurgeUtil {

    class Url {
    public:
        Url(const std::string &url);

        std::string GetProtocol();
        std::string GetHost();
        int GetPort();
        std::string GetFullPath();
        std::string GetPath();

        std::string AsString();
        std::string WithRtspProtocol();

    public:
        bool IsSecure() {
            return SurgeUtil::String::Contains(protocol, SECURE_RTSP_PROTOCOL);
        }

        bool IsIPv6() {
            return IpIsIPv6(ip);
        }

    public:
        static bool IpIsIPv6(const std::string &ip) {
            return SurgeUtil::String::Contains(ip, ":");
        }

    private:
        std::string protocol;
        std::string ip;
        std::string port;
        std::string path;
        std::string queryParameters;
    };
}

#endif //__URL_H__
