// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
