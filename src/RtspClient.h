// -*-c++-*-
#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "SocketHandler.h"
#include "DescribeResponse.h"
#include "SetupResponse.h"

#include <string>

namespace Surge {

    class RtspClient {

    public:
        RtspClient();

        ~RtspClient();

        DescribeResponse* Describe(const std::string url,
                                   bool requires_auth,
                                   const std::string user,
                                   const std::string password);

        SetupResponse* Setup();

        RtspResponse* Options();
        
        void StopClient();

    private:

        int GetNextSequenceNumber() { return m_sequenceNumber++; }

        int m_keeepAliveIntervalInSeconds;
        int m_sequenceNumber;
        std::string m_url;
        std::string m_session;        
        Surge::SocketHandler m_socketHandler;
    };
    
}

#endif //__RTSP_CLIENT_H__
