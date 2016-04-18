// -*-c++-*-
// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
                                             m_timeout(DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS) {
            
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
            std::string value = SurgeUtil::FindKeyAndValuePair(&values, "interleaved");
            if (!value.empty()) {
                m_isInterleaved = true;
                
                values = SurgeUtil::StringSplit(value, '=');
                values = SurgeUtil::StringSplit(values[1], '-');
                
                m_rtpInterleavedChannel = atoi(values[0].c_str());
                m_rtcpInterleavedChannel = atoi(values[1].c_str());
            }
        }

        int GetRtpInterleavedChannel() const { return m_rtpInterleavedChannel; }

        int GetRtcpInterleavedChannel() const { return m_rtcpInterleavedChannel; }

        const std::string GetSession() const { return m_session; }

        int GetTimeoutSeconds() const { return m_timeout; }

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
