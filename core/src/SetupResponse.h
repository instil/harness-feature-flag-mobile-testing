// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __SETUP_RESPONSE_H__
#define __SETUP_RESPONSE_H__

#include "RtspResponse.h"
#include "Helpers.h"
#include "Constants.h"

#include <cstdlib>

using SurgeUtil::Constants::DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS;

namespace Surge {

    class SetupResponse: public RtspResponse {
    public:
        SetupResponse(const Response *resp): RtspResponse(resp),
                                             m_isInterleaved(false),
                                             m_rtpInterleavedChannel(0),
                                             m_rtcpInterleavedChannel(1),
                                             m_session(),
                                             m_timeout(DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS)
        {
            
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
                    m_timeout = atoi(values[1].c_str());
                }
            } else {
                // simply just session
                m_session = values[0];
            }

            values = SurgeUtil::StringSplit(transport_header, ';');
            std::string interleavedValue = SurgeUtil::FindKeyAndValuePair(&values, "interleaved");
            std::string serverPortValue = SurgeUtil::FindKeyAndValuePair(&values, "server_port");
            if (!interleavedValue.empty()) {
                m_isInterleaved = true;
                
                values = SurgeUtil::StringSplit(interleavedValue, '=');
                values = SurgeUtil::StringSplit(values[1], '-');
                
                m_rtpInterleavedChannel = atoi(values[0].c_str());
                m_rtcpInterleavedChannel = atoi(values[1].c_str());
            } else if (!serverPortValue.empty()) {
                values = SurgeUtil::StringSplit(serverPortValue, '=');
                values = SurgeUtil::StringSplit(values[1], '-');
                
                m_rtpServerPort = atoi(values[0].c_str());
                m_rtcpServerPort = atoi(values[1].c_str());
            }
        }

        int GetRtpInterleavedChannel() const { return m_rtpInterleavedChannel; }

        int GetRtcpInterleavedChannel() const { return m_rtcpInterleavedChannel; }

        const std::string GetSession() const { return m_session; }

        int GetTimeoutSeconds() const { return m_timeout; }

        bool IsInterleaved() const { return m_isInterleaved; }
        
        int GetRtpServerPort() const { return m_rtpServerPort; }
        
        int GetRtcpServerPort() const { return m_rtcpServerPort; }

    private:
        bool m_isInterleaved;
        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;
        std::string m_session;
        int m_timeout;
        
        int m_rtpServerPort;
        int m_rtcpServerPort;
    };
    
};

#endif //__SETUP_RESPONSE_H__
