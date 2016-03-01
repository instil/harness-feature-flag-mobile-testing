// -*-c++-*-
#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "SocketHandler.h"
#include "DescribeResponse.h"

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

        void StopClient();

    private:
        Surge::SocketHandler m_socketHandler;
    };
    
}

#endif //__RTSP_CLIENT_H__
