// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "SessionDescriptionV0.h"

Surge::SessionDescriptionV0::SessionDescriptionV0(const std::string& rawSessionDescription): SessionDescription()
{
    bool isVideo = false;
    
    std::vector<std::string> lines = SurgeUtil::StringSplit(rawSessionDescription, "\r\n");
    for (auto it = lines.begin(); it != lines.end(); ++it) {
        const std::string currentLine = *it;

        bool isStartOfNewStream = currentLine.find("m=") != std::string::npos;
        bool isVideoStream = currentLine.find("m=video") != std::string::npos;
        bool isRtpmap = currentLine.find("a=rtpmap") != std::string::npos;
        bool isFmtp = currentLine.find("a=fmtp") != std::string::npos;
        bool isControl = currentLine.find("a=control") != std::string::npos;
        bool isMime = isRtpmap;
        
        if (isStartOfNewStream) {
            isVideo = isVideoStream;
        }

        if (isVideo) {
            if (isRtpmap) {
                rtpMap = currentLine;
            }

            if (isFmtp) {
                formatParameters = currentLine;
            }

            if (isControl) {
                size_t end_position = currentLine.find(';');
                control = currentLine.substr(10, end_position - 10);
            }

            if (isMime) {
                type = GetTypeFromMime(currentLine);
            }
        }
    }
}
