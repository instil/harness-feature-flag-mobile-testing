// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "RtpPacket.h"
#include "Logging.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <functional>

// ntohs..
#include <arpa/inet.h>

namespace Surge {
    
    class Response {
    public:
        Response(const unsigned char *buffer, size_t length): m_buffer(nullptr),
                                                              m_length(length) {
            
            unsigned char *copy = (unsigned char*)malloc(length);
            memcpy((void*)copy, (const void *)buffer, length);
            m_buffer = copy;
        }

        ~Response() {
            free(m_buffer);
        }

        const unsigned char *BytesPointer() const { return m_buffer; }
        
        const size_t PointerLength() const { return m_length; }

        const std::string StringDump() const {
            size_t length = PointerLength();
            char *dump = (char*)malloc(length + 1);
            
            memset((void*)dump, 0, length + 1);
            memcpy((void*)dump, (const void *)BytesPointer(), length);

            std::string repr = std::string(dump);
            free(dump);
            
            return repr;
        }

        std::string StringDump() {
            char *buffer = (char *)malloc(PointerLength() + 1);
            memset(buffer, 0, PointerLength() + 1);
            memcpy(buffer, m_buffer, m_length);
            
            std::string dump = std::string("[") + std::string(buffer) + std::string("]");
            free(buffer);
            return dump;
        }

    private:       
        unsigned char *m_buffer;
        size_t m_length;
    };  
    
}

#endif //__RESPONSE_H__
