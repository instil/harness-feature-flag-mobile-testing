// -*-c++-*-
#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <string>

namespace SurgeUtil {
    
    namespace Constants {

        static int DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS = 60;

        static int DEFAULT_NO_PACKET_TIMEOUT_MS = 5000;

        static int DEFAULT_CONNECT_TIMEOUT_MS = 5000;

        static int DEFAULT_TRANSACTION_TIMEOUT_MS = 5000;

        static int DEFAULT_RTP_INTERLEAVED_CHANNEL = 0;

        static int DEFAULT_RTCP_INTERLEAVED_CHANNEL = 1;

        static int DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE = 256 * 1024;

        static int DEFAULT_SOCKET_HANDLER_TIMEOUT_MS = 5000;
        
    }
    
}

#endif //__CONSTANTS_H__
