// -*-c++-*-
#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include <string>

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
    };
    
}

#endif //__RTSP_CLIENT_H__
