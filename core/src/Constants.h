// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <string>

namespace SurgeUtil {
    
    namespace Constants {

        static int DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS = 60;

        static int DEFAULT_NO_PACKET_TIMEOUT_MS = 10000;

        static int DEFAULT_CONNECT_TIMEOUT_MS = 5000;

        static int DEFAULT_TRANSACTION_TIMEOUT_MS = 5000;
        
        static int DEFAULT_PACKET_BUFFER_DELAY_MS = 200;

        static int DEFAULT_RTP_INTERLEAVED_CHANNEL = 0;

        static int DEFAULT_RTCP_INTERLEAVED_CHANNEL = 1;

        static int DEFAULT_RTP_PORT = 5000;

        static int DEFAULT_RTCP_PORT = 5001;

        static int DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE = 256 * 1024;

        static int DEFAULT_SOCKET_HANDLER_TIMEOUT_MS = 5000;
        
    }
    
}

#endif //__CONSTANTS_H__
