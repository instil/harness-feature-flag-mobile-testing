// -*-c++-*-
#ifndef __SESSION_DESCRIPTION_H__
#define __SESSION_DESCRIPTION_H__

#include "MimeTypes.h"

#include <string>


namespace Surge {

    class SessionDescription {
    public:

        RtspSessionType GetType() const { return m_type; }

        bool IsControlUrlComplete() const { return m_controlUrlIsComplete; }

        const std::string GetControl() const { return m_control; }

        const std::string GetRtpMap() const { return m_rtpMap; }

        const std::string GetFmtp() const { return m_fmtp; }

    protected:
        
        bool m_controlUrlIsComplete;
        RtspSessionType m_type;
        std::string m_control;
        std::string m_rtpMap;
        std::string m_fmtp;
    };
    
}

#endif //__SESSION_DESCRIPTION_H__
