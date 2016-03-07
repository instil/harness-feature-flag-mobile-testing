// -*-c++-*-
#ifndef __SESSION_DESCRIPTION_H__
#define __SESSION_DESCRIPTION_H__

#include "MimeTypes.h"
#include "Logging.h"

#include <string>
#include <vector>


namespace Surge {

    class SessionDescription {
    public:

        virtual ~SessionDescription() { }

        RtspSessionType GetType() const { return m_type; }

        bool IsControlUrlComplete() const { return m_controlUrlIsComplete; }

        const std::string GetControl() const { return m_control; }

        const std::string GetRtpMap() const { return m_rtpMap; }

        const std::string GetFmtp() const { return m_fmtp; }

        const std::string GetFmtpH264ConfigParameters() const {
            if (m_type != RtspSessionType::H264) {
                return std::string();
            }
            
            size_t pos = m_fmtp.find("sprop-parameter-sets=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = m_fmtp.find(";", pos);
            if (end == std::string::npos) {
                end = m_fmtp.length();
            }
            
            return m_fmtp.substr(pos + 21, end);
        }

    protected:        
        bool m_controlUrlIsComplete;
        RtspSessionType m_type;
        std::string m_control;
        std::string m_rtpMap;
        std::string m_fmtp;
    };
    
}

#endif //__SESSION_DESCRIPTION_H__
