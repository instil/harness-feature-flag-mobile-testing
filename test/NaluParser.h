// -*-c++-*-
#ifndef __NALU_PARSER_H__
#define __NALU_PARSER_H__

#include "Logging.h"
#include <string>

namespace SurgeTestUtil {
    
    void PrintOutAllNaluTypes(const unsigned char *buffer, size_t length) {
        std::string nalus;
        
        size_t i;
        for (i = 0; i < length - 4; ++i) {
            if (buffer[i] == 0x00 && buffer[i+1] == 0x00 && buffer[i+2] == 0x01) {
                int nalu_type = (buffer[i + 3] & 0x1F);
                nalus += "[" + std::to_string(nalu_type) + "] ";
            }
        }

        INFO("NALUS: " << nalus);
    }
    
};

#endif //__NALU_PARSER_H__
