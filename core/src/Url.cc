//
//  Url.cpp
//  SurgeCore
//
//  Created by Paul Shields on 04/09/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#include "Url.h"

SurgeUtil::Url::Url(const std::string &url) {
    std::vector<std::string> result = SurgeUtil::Regex::Split(url, URL_MATCH_REGEX);

    if (!SurgeUtil::String::Contains(result[1], RTSP_PROTOCOL)) {
        return;
    }

    protocol = result[1];
    ip = result[2];
    port = result[3];
    path = result[4];
    queryParameters = result[5];

    if (SurgeUtil::String::Contains(ip, "[")) {
        ip = ip.substr(1, ip.length() - 2);
    }

    if (port.length() == 0) {
        port = DEFAULT_RTSP_PORT;
    }
}

std::string SurgeUtil::Url::GetProtocol() {
    return protocol;
}

std::string SurgeUtil::Url::GetHost() {
    return ip;
}

int SurgeUtil::Url::GetPort() {
    return atoi(port.c_str());
}

std::string SurgeUtil::Url::GetFullPath() {
    return path + queryParameters;
}

std::string SurgeUtil::Url::GetPath() {
    return path;
}

std::string SurgeUtil::Url::AsString() {
    return GetProtocol() + "://" + GetHost() + ":" + std::to_string(GetPort()) + "/" + GetFullPath();
}

std::string SurgeUtil::Url::WithRtspProtocol() {
    return std::string(RTSP_PROTOCOL) + "://" + GetHost() + ":" + std::to_string(GetPort()) + "/" + GetFullPath();
}
