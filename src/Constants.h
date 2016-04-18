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

        static int DEFAULT_RTP_PORT = 5000;

        static int DEFAULT_RTCP_PORT = 5001;

        static int DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE = 256 * 1024;

        static int DEFAULT_SOCKET_HANDLER_TIMEOUT_MS = 5000;
        
    }
    
}

#endif //__CONSTANTS_H__
