// -*-c++-*-
#ifndef __MP4V_DEPACKETIZER_H__
#define __MP4V_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"

#include <vector>

namespace Surge {

    class MP4VDepacketizer {
    public:

        MP4VDepacketizer(const SessionDescription* palette, const RtpPacket *packet, bool isFirstPayload);

        const unsigned char *PayloadBytes() const { return &(m_payload[0]); }

        size_t PayloadLength() const { return m_payload.size(); }

    private:

        void PushBytesToCurrentPayload(const unsigned char *payload, size_t length) {
            size_t i;
            for (i = 0; i < length; ++i) {
                m_payload.push_back(payload[i]);
            }
        }
        
        const SessionDescription *m_palette;
        const RtpPacket *m_packet;

        std::vector<unsigned char> m_payload;
    };
    
};

#endif //__MP4V_DEPACKETIZER_H__
