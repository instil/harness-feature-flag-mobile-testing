#include "MP4VDepacketizer.h"

#include <cstdlib>

Surge::MP4VDepacketizer::MP4VDepacketizer(const SessionDescription* palette,
                                          const RtpPacket *packet,
                                          bool isFirstPayload) : m_palette(palette),
                                                                 m_packet(packet) {
    if (isFirstPayload) {
        std::string config = m_palette->GetFmtpConfigParameters();
        std::vector<unsigned char> config_bytes;

        size_t i;
        for (i = 0; i < config.length(); i += 2) {
            std::string current_byte = config.substr(i, 2);
            unsigned char wrapped_byte = (unsigned char)strtol(current_byte.c_str(), NULL, 16);
            config_bytes.push_back(wrapped_byte);
        }

        PushBytesToCurrentPayload(&(config_bytes[0]), config_bytes.size());
    }

    const unsigned char *rtp_packet_payload = m_packet->PayloadData();
    size_t rtp_packet_payload_length = m_packet->PayloadLength();

    PushBytesToCurrentPayload(rtp_packet_payload, rtp_packet_payload_length);
}
