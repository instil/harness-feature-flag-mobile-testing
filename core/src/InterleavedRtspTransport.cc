// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "InterleavedRtspTransport.h"

using SurgeUtil::Constants::DEFAULT_RTP_INTERLEAVED_CHANNEL;
using SurgeUtil::Constants::DEFAULT_RTCP_INTERLEAVED_CHANNEL;
using SurgeUtil::Constants::DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE;

Surge::InterleavedRtspTransport::InterleavedRtspTransport(ISocketHandlerDelegate * const delegate, TransportDelegate * const transportDelegate):
Transport(delegate),
m_readBufferSize(DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE),
m_rtpInterleavedChannel(DEFAULT_RTP_INTERLEAVED_CHANNEL),
m_rtcpInterleavedChannel(DEFAULT_RTCP_INTERLEAVED_CHANNEL),
transportDelegate(transportDelegate) { }

bool Surge::InterleavedRtspTransport::HandlePacket(const char* buffer, size_t size) {
    if (!BufferContainsRtpPacket()) {
        return false;
    }
    
    int channel = static_cast<int>(m_receivedBuffer[1]);
    
    uint16_t network_order_packet_length = 0;
    memcpy(&network_order_packet_length, &(m_receivedBuffer[2]), 2);
    uint16_t packet_length = ntohs(network_order_packet_length);
    
    bool have_whole_packet = (m_receivedBuffer.size() - (4)) >= packet_length;
    
    if (have_whole_packet && channel == m_rtpInterleavedChannel) {
        try {
            if (rtpCallback != nullptr) {
                RtpPacket* pack = new RtpPacket(&(m_receivedBuffer[4]), packet_length);
                transportDelegate->RtpPacketReceived(pack);
            } else {
                ERROR("RTP packet available but callback is null!");
            }
        } catch (const std::exception& e) {
            ERROR("Invalid Rtp Packet: " << e.what());
        }
    } else if (!have_whole_packet) {
        return false;
    }
    
    RemoveDataFromStartOfBuffer(packet_length + 4);
    
    return true;
}
