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
                                             m_timeout(DEFAULT_KEEP_ALIVE_INTERVAL_MS) {
            
            const std::string session_header = HeaderValueForKey("Session");
            
            if (!session_header.empty()) {
                INFO("SETUP VALUE: " << session_header);
                std::vector<std::string> values = SurgeUtil::StringSplit(session_header, ';');
                
                if (values.size() > 1) {
                    // we probably have session;timeout=
                    
                } else {
                    // simply just session

                    
                }
            }
        }

        const std::string GetSession() const { return m_session; }

        int GetTimeout() const { return m_timeout; }

    private:
        std::string m_session;
        int m_timeout;
    };
    
};

#endif //__SETUP_RESPONSE_H__
