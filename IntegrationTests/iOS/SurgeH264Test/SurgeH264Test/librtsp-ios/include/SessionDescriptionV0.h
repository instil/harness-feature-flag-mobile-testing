// -*-c++-*-
#ifndef __SESSION_DESCRIPTION_V0_H__
#define __SESSION_DESCRIPTION_V0_H__

#include "SessionDescription.h"
#include "MimeTypes.h"
#include "Helpers.h"
#include "Logging.h"

#include <string>


namespace Surge {

    class SessionDescriptionV0: public SessionDescription {
    public:
        SessionDescriptionV0(const std::string rawSessionDescription) {
            m_controlUrlIsComplete = false;
            m_type = RtspSessionType::H264;

            std::vector<std::string> lines = SurgeUtil::StringSplit(rawSessionDescription, "\r\n");
            for (auto it = lines.begin(); it != lines.end(); ++it) {
                const std::string current_line = *it;

                bool is_rtpmap = current_line.find("a=rtpmap") != std::string::npos;
                bool is_fmtp = current_line.find("a=fmtp") != std::string::npos;
                bool is_control = current_line.find("a=control") != std::string::npos;

                if (is_rtpmap) {
                    m_rtpMap = current_line;
                }
                else if (is_fmtp) {
                    m_fmtp = current_line;
                }
                else if (is_control) {
                    m_control = current_line.substr(10, current_line.length());
                }
            }
        }

    };
    
}

#endif //__SESSION_DESCRIPTION_V0_H__
