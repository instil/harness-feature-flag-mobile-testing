// -*-c++-*-
#ifndef __SETUP_RESPONSE_H__
#define __SETUP_RESPONSE_H__

#include "RtspResponse.h"
#include "Helpers.h"
#include "Constants.h"

using SurgeUtil::Constants::DEFAULT_KEEP_ALIVE_INTERVAL_MS;

namespace Surge {

    class SetupResponse: public RtspResponse {
    public:
        SetupResponse(const Response *resp): RtspResponse(resp),
                                             m_isInterleaved(false),
                                             m_rtpInterleavedChannel(0),
                                             m_rtcpInterleavedChannel(1),
                                             m_session(),
                                             m_timeout(DEFAULT_KEEP_ALIVE_INTERVAL_MS) {
            
            const std::string session_header = HeaderValueForKey("Session");
            const std::string transport_header = HeaderValueForKey("Transport");
            
            if (session_header.empty() || transport_header.empty()) {
                ostringstream message;
                message << "Invalid Setup RTSP Response";
                throw runtime_error{ message.str() };
            }
            
            std::vector<std::string> values = SurgeUtil::StringSplit(session_header, ';');
            if (values.size() > 1) {
                // we probably have session;timeout=
                m_session = values[0];
                values = SurgeUtil::StringSplit(values[1], "=");
                
                if (values.size() > 1) {
                    int numerical_base = 10;
                    m_timeout = std::stoi(values[1], nullptr, numerical_base);
                }
            } else {
                // simply just session
                m_session = values[0];
            }

            values = SurgeUtil::StringSplit(transport_header, ';');
            std::string value = SurgeUtil::FindKeyAndValuePair(&values, "interleaved");
            if (!value.empty()) {
                m_isInterleaved = true;
                
                values = SurgeUtil::StringSplit(value, '=');
                values = SurgeUtil::StringSplit(values[1], '-');
                
                int numerical_base = 10;
                m_rtpInterleavedChannel = std::stoi(values[0], nullptr, numerical_base);
                m_rtcpInterleavedChannel = std::stoi(values[1], nullptr, numerical_base);
            }
        }

        int GetRtpInterleavedChannel() const { return m_rtpInterleavedChannel; }

        int GetRtcpInterleavedChannel() const { return m_rtcpInterleavedChannel; }

        const std::string GetSession() const { return m_session; }

        int GetTimeout() const { return m_timeout; }

        bool IsInterleaved() const { return m_isInterleaved; }

    private:
        bool m_isInterleaved;
        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;
        std::string m_session;
        int m_timeout;
    };
    
};

#endif //__SETUP_RESPONSE_H__
