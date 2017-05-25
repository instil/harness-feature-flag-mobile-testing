// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __PAUSE_H__
#define __PAUSE_H__

#include "RtspCommand.h"

namespace Surge {

    class PauseRequest: public RtspCommand {
    public:
        PauseRequest(const std::string& url,
                     const std::string& session,
                     const int nextSequenceNumber,
                     const std::string& authHeader) {
            std::string packet = "PAUSE " + url + " RTSP/1.0\r\n";
            
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

        ~PauseRequest() {
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
    
}

#endif //__PAUSE_H__
