// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "SessionDescriptionV0.h"

Surge::SessionDescriptionV0::SessionDescriptionV0(const std::string& rawSessionDescription): SessionDescription()
{
    std::vector<std::string> lines = SurgeUtil::StringSplit(rawSessionDescription, "\r\n");
    for (auto it = lines.begin(); it != lines.end(); ++it) {
        const std::string current_line = *it;

        bool is_rtpmap = current_line.find("a=rtpmap") != std::string::npos;
        bool is_fmtp = current_line.find("a=fmtp") != std::string::npos;
        bool is_control = current_line.find("a=control") != std::string::npos;
        bool is_mime = is_rtpmap;

        if (is_rtpmap) {
            rtpMap = current_line;
        }
                
        if (is_fmtp) {
            formatParameters = current_line;
        }
                
        if (is_control) {
            size_t end_position = current_line.find(';');
            control = current_line.substr(10, end_position - 10);
        }
                
        if (is_mime) {    
            type = GetTypeFromMime(current_line);
        }
    }
}
