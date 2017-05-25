// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "SessionDescriptionFactory.h"
#include "SessionDescriptionV0.h"

#include "Helpers.h"
#include "Logging.h"



std::vector<Surge::SessionDescription> Surge::SessionDescriptionFactory::ParseSessionDescriptionsFromBuffer(const std::string& body) {
    std::vector<SessionDescription> sessionDescriptions;

    std::vector<std::string> lines = SurgeUtil::StringSplit(body, "\n");
    if (lines.size() > 0) {
        std::string version_line = lines[0];

        if (!version_line.empty()) {
            sessionDescriptions = ParseSessionDescription(version_line, body);
        } else {
            ERROR("No Session Description available");
        }
    }
    
    return sessionDescriptions;
}

std::vector<Surge::SessionDescription> Surge::SessionDescriptionFactory::ParseSessionDescription(const std::string& versionLine, const std::string& body) {
    std::vector<SessionDescription> sessionDescriptions;
        
    if (versionLine.find("v=0") != std::string::npos) {
        SessionDescriptionV0 sessionDescription(body);
        sessionDescriptions.push_back(sessionDescription);
    }
    else {
        ERROR("Unhandled Session Description version: [" << versionLine << "]");
    }
    
    return sessionDescriptions;
}
