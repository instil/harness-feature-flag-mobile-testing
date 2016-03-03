// -*-c++-*-
#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "RtpPacket.h"
#include "Logging.h"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace Surge {

    static int packet_index = 0;
    
    class Response {
    public:
        Response(unsigned char *buffer, size_t length): m_buffer(buffer),
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

        bool IsInterleavedPacket() const { return m_buffer[0] == '$'; }

        const std::string StringDump() const {
            size_t length = PointerLength();
            char *dump = (char*)malloc(length + 1);
            
            memset((void*)dump, 0, length + 1);
            memcpy((void*)dump, (const void *)BytesPointer(), length);

            std::string repr = std::string(dump);
            free(dump);
            
            return repr;
        }

        const std::string GetNewFile() {
            return "/tmp/rtp_packet." + std::to_string(packet_index++) + ".dat";
        }

        RtpPacket *GetRawRtpPacketFromInterleaved() {
            if (!IsInterleavedPacket()) {
                return nullptr;
            }

            const char *file = GetNewFile().c_str();
            INFO("TMP FILE: " << file);
            FILE *fd = fopen(file, "w");
            fwrite(m_buffer, 1, m_length, fd);
            fclose(fd);
            
            uint16_t packet_length_network_order;
            memcpy(&packet_length_network_order, m_buffer + 2, 2);
            uint16_t packet_length = ntohs(packet_length_network_order);

            INFO("TOTAL: " << m_length << " - RTP: " << packet_length);
            
            return new RtpPacket(m_buffer + 4, packet_length);
        }

        int GetInterleavedPacketChannelNumber() {
            if (!IsInterleavedPacket()) {
                return -1;
            }

            unsigned char channel_number = m_buffer[1];
            return static_cast<int>(channel_number);
        }

    private:
       
        unsigned char *m_buffer;
        size_t m_length;
    };  
    
}

#endif //__RESPONSE_H__
