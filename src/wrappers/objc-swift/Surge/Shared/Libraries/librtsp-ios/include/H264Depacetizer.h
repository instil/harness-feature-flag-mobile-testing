// -*-c++-*-
#ifndef __H264_DEPACKETIZER_H__
#define __H264_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"

#include <vector>

namespace Surge {

    class H264Depacketizer {
    public:

        H264Depacketizer(const SessionDescription* palette, const RtpPacket *packet, bool isFirstPayload);

        const unsigned char *PayloadBytes() const { return &(m_payload[0]); }

        size_t PayloadLength() const { return m_payload.size(); }

    private:
        int GetH264NaluTypeFromByte(const unsigned char byte) const {
            return byte & 0x1F;
        }
        
        void PushBytesToCurrentPayload(const unsigned char *bytes, size_t length) {
            size_t i;
            for (i = 0; i < length; ++i) {
                m_payload.push_back(bytes[i]);
            }
        }

        void Push4ByteNaluHeaderToCurrentPayload() {
            unsigned char nalu_header[] = { 0x00, 0x00, 0x00, 0x01 };
            PushBytesToCurrentPayload(nalu_header, sizeof(nalu_header));
        }

        void Push3ByteNaluHeaderToCurrentPayload() {
            unsigned char nalu_header[] = { 0x00, 0x00, 0x01 };
            PushBytesToCurrentPayload(nalu_header, sizeof(nalu_header));
        }
        
        const SessionDescription *m_palette;
        const RtpPacket *m_packet;

        std::vector<unsigned char> m_payload;
    };
    
};

#endif //__H264_DEPACKETIZER_H__
