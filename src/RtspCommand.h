// -*-c++-*-
#ifndef __RTSP_COMMAND_H__
#define __RTSP_COMMAND_H__

#include <cstdlib>

namespace Surge {

    class RtspCommand {
    public:

        virtual ~RtspCommand() { };

        virtual const unsigned char * BytesPointer() = 0;
 
        virtual size_t PointerLength() = 0;
    };
    
}

#endif //__RTSP_COMMAND_H__
