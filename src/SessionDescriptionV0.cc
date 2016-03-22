#include "SessionDescriptionV0.h"

Surge::SessionDescriptionV0::SessionDescriptionV0(const std::string rawSessionDescription): SessionDescription()
{
    std::vector<std::string> lines = SurgeUtil::StringSplit(rawSessionDescription, "\r\n");
    for (auto it = lines.begin(); it != lines.end(); ++it) {
        const std::string current_line = *it;

        bool is_rtpmap = current_line.find("a=rtpmap") != std::string::npos;
        bool is_fmtp = current_line.find("a=fmtp") != std::string::npos;
        bool is_control = current_line.find("a=control") != std::string::npos;
        bool is_mime = is_rtpmap;

        if (is_rtpmap) {
            m_rtpMap = current_line;
        }
                
        if (is_fmtp) {
            m_fmtp = current_line;
        }
                
        if (is_control) {
            m_control = current_line.substr(10, current_line.length());
        }
                
        if (is_mime) {    
            m_type = GetTypeFromMime(current_line);
        }
    }
}
