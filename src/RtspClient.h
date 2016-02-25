// -*-c++-*-
#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include <string>

#include "SocketHandler.h"

namespace Surge {

    class RtspClient {

    public:
        RtspClient();

        ~RtspClient();

        int Describe(const std::string url,
                     bool requires_auth,
                     const std::string user,
                     const std::string password);

        void StopClient();

    private:

        Surge::SocketHandler m_socketHandler;
    };
    
}

#endif //__RTSP_CLIENT_H__
