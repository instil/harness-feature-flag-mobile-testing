// -*-c++-*-
#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include <string>

namespace Surge {

    class RtspClient {

        RtspClient();

        ~RtspClient();

        int Describe(const std::string url);

        void StopClient();
    };
    
}

#endif //__RTSP_CLIENT_H__
