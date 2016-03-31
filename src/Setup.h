// -*-c++-*-
#ifndef __SETUP_H__
#define __SETUP_H__

#include "RtspCommand.h"

#include <cstdlib>
#include <string>

namespace Surge {

    class SetupRequest: public RtspCommand {
    public:
        SetupRequest(const std::string& url,
                     const int nextSequenceNumber,
                     const std::string& authHeader) {
            
            std::string packet = "SETUP " + url + " RTSP/1.0\r\n";
            
            char headerField[1024];
            snprintf(headerField, sizeof(headerField),
                     "CSeq: %d\r\n", nextSequenceNumber);
            packet += std::string(headerField);

            packet += "Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n";

            if (!authHeader.empty()) {
                packet += authHeader;
            }
            packet += "\r\n";

            m_buffer = (unsigned char *)malloc(packet.length());
            m_length = packet.copy((char *)m_buffer, packet.length(), 0);
        }

        ~SetupRequest() {
            free(m_buffer);
        }

        const unsigned char * BytesPointer() const override {
            return m_buffer;
        }

        size_t PointerLength() const override {
            return m_length;
        }

    private:       
        unsigned char *m_buffer;
        size_t m_length;
    };
    
};

#endif //__SETUP_H__
