#include "H264Depacetizer.h"
#include "Base64.h"

Surge::H264Depacketizer::H264Depacketizer(const SessionDescription* palette,
                                          const RtpPacket *packet,
                                          bool isFirstPayload) : m_palette(palette),
                                                                 m_packet(packet)
{
    const unsigned char *rtp_packet_payload = m_packet->PayloadData();
    size_t rtp_packet_payload_length = m_packet->PayloadLength();

    if (isFirstPayload) {
        // append fmtp nalu config lines
        std::string config = m_palette->GetFmtpH264ConfigParameters();

        size_t pos = config.find(",");
        std::string first_nalu = config.substr(0, pos);
        std::string second_nalu = config.substr(pos + 1, config.length());

        // They are base 64 encoded
        std::string first_nalu_decoded = SurgeUtil::Base64Decode(first_nalu);
        std::string second_nalu_decoded = SurgeUtil::Base64Decode(second_nalu);
        
        PushNaluHeaderToCurrentPayload();
        PushBytesToCurrentPayload((const unsigned char *)first_nalu_decoded.c_str(),
                                  first_nalu_decoded.length());

        PushNaluHeaderToCurrentPayload();
        PushBytesToCurrentPayload((const unsigned char *)second_nalu_decoded.c_str(),
                                  second_nalu_decoded.length());
    }

    int payload_type = GetH264NaluTypeFromByte(rtp_packet_payload[0]);

    switch(payload_type) {
    case 0:
        PushBytesToCurrentPayload(rtp_packet_payload, rtp_packet_payload_length);
        break;

    case 7:
    case 8:
        PushNaluHeaderToCurrentPayload();
        PushBytesToCurrentPayload(rtp_packet_payload, rtp_packet_payload_length);
        break;
        
    case 24:
        PushNaluHeaderToCurrentPayload();
        PushBytesToCurrentPayload(rtp_packet_payload + 1, rtp_packet_payload_length - 1);
        break;

    case 25:
    case 26:
    case 27:
        PushNaluHeaderToCurrentPayload();
        PushBytesToCurrentPayload(rtp_packet_payload + 3, rtp_packet_payload_length - 3);
        break;

    case 28:
    case 29: {
        unsigned char start_bit = rtp_packet_payload[1] >> 7;
        if (start_bit) {
            unsigned char *temp_payload = (unsigned char*)malloc(rtp_packet_payload_length - 1);
            memcpy(temp_payload, rtp_packet_payload + 1, rtp_packet_payload_length - 1);
            
            unsigned char header = (rtp_packet_payload[0] & 0xE0) + (rtp_packet_payload[1] & 0x1F);
            temp_payload[0] = header;
            
            PushNaluHeaderToCurrentPayload();
            PushBytesToCurrentPayload(temp_payload, rtp_packet_payload_length - 1);
            
            free(temp_payload);
        } else {
            PushBytesToCurrentPayload(rtp_packet_payload + 2, rtp_packet_payload_length - 2);
        }
    }
        break;
        
    default:
        PushNaluHeaderToCurrentPayload();
        PushBytesToCurrentPayload(rtp_packet_payload, rtp_packet_payload_length);
        break;
    }
}
