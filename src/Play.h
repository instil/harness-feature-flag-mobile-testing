// -*-c++-*-
#ifndef __PLAY_H__
#define __PLAY_H__

#include "RtspCommand.h"

#include <cstdlib>
#include <string>

namespace Surge {

    class PlayRequest: public RtspCommand {
    public:
        PlayRequest(const std::string& url,
                    const std::string& session,
                    const int nextSequenceNumber,
                    const std::string& authHeader) {

            std::string packet = "PLAY " + url + " RTSP/1.0\r\n";
            
            char headerField[256];
            snprintf(headerField, sizeof(headerField),
                     "CSeq: %d\r\n", nextSequenceNumber);
            packet += std::string(headerField);

            packet += "Session: " + session + "\r\n";

            packet += "Range: npt=0.000-\r\n";

            if (!authHeader.empty()) {
                packet += authHeader;
            }
            packet += "\r\n";

            m_buffer = (unsigned char *)malloc(packet.length());
            m_length = packet.copy((char *)m_buffer, packet.length(), 0);
        }

        ~PlayRequest() {
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

#endif //__PLAY_H__
