// -*-c++-*-
#ifndef __RTSP_COMMAND_H__
#define __RTSP_COMMAND_H__

#include <cstdlib>

#include <string>

namespace Surge {

    class RtspCommand {
    public:

        virtual ~RtspCommand() { };

        virtual const unsigned char * BytesPointer() const = 0;
 
        virtual size_t PointerLength() const = 0;

        const std::string StringDump() const {
            size_t length = this->PointerLength();
            char *dump = (char*)malloc(length + 1);
            memset((void*)dump, 0, length + 1);
            memcpy((void*)dump, (const void *)this->BytesPointer(), length);

            std::string repr = std::string(dump);
            free(dump);
            return repr;            
        }
    };
    
}

#endif //__RTSP_COMMAND_H__
