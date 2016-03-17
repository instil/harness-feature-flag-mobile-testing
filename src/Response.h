// -*-c++-*-
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

        std::vector<RtpPacket*> GetRtpPackets() {
            std::vector<RtpPacket*> packets;

            size_t packet_offset = 0;
            bool has_more_packets = false;

            do {                
                uint16_t packet_length_network_order;
                memcpy(&packet_length_network_order, m_buffer + packet_offset + 2, 2);
                uint16_t packet_length = ntohs(packet_length_network_order);

                RtpPacket *pack =  new RtpPacket(m_buffer + packet_offset + 4, packet_length);
                packets.push_back(pack);

                packet_offset += 4 + packet_length;
                has_more_packets = packet_offset < m_length;
            } while (has_more_packets);

            return packets;
        }

        void ParseRtpPackets(std::function<void (RtpPacket *)> handler) {
            size_t packet_offset = 0;
            bool has_more_packets = false;

            do {                
                uint16_t packet_length_network_order;
                memcpy(&packet_length_network_order, m_buffer + packet_offset + 2, 2);
                uint16_t packet_length = ntohs(packet_length_network_order);

                RtpPacket *pack =  new RtpPacket(m_buffer + packet_offset + 4, packet_length);

                handler(pack);

                packet_offset += 4 + packet_length;
                has_more_packets = packet_offset < m_length;
            } while (has_more_packets);
        }

        int GetInterleavedPacketChannelNumber() {
            if (!IsInterleavedPacket()) {
                return -1;
            }

            unsigned char channel_number = m_buffer[1];
            return static_cast<int>(channel_number);
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
