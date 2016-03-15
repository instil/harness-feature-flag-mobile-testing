// -*-c++-*-
#ifndef __TEARDOWN_H__
#define __TEARDOWN_H__

#include "RtspCommand.h"

namespace Surge {

    class TeardownRequest: public RtspCommand {
    public:
        TeardownRequest(const std::string url,
                        const std::string session,
                        const int nextSequenceNumber,
                        const std::string authHeader) {
            std::string packet = "TEARDOWN " + url + " RTSP/1.0\r\n";
            
            char headerField[1024];
            snprintf(headerField, sizeof(headerField),
                     "CSeq: %d\r\n", nextSequenceNumber);
            packet += std::string(headerField);

            packet += "Session: " + session + "\r\n";

            if (!authHeader.empty()) {
                packet += authHeader;
            }
            packet += "\r\n";

            m_buffer = (unsigned char *)malloc(packet.length());
            m_length = packet.copy((char *)m_buffer, packet.length(), 0);
        }

        ~TeardownRequest() {
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

#endif //__TEARDOWN_H__