// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef InterleavedRtspTransport_h
#define InterleavedRtspTransport_h

#include "Transport.h"
#include "Constants.h"

namespace Surge {
    class InterleavedRtspTransport : public Surge::Transport {
    public:
        InterleavedRtspTransport(ISocketHandlerDelegate * const delegate);
        
        std::string GetTransportHeaderString() const override {
            char rtp_channel[12];
            char rtcp_channel[12];
            
            memset(rtp_channel, 0, sizeof(rtp_channel));
            memset(rtcp_channel, 0, sizeof(rtcp_channel));
            snprintf(rtp_channel, sizeof(rtp_channel), "%d", m_rtpInterleavedChannel);
            snprintf(rtcp_channel, sizeof(rtcp_channel), "%d", m_rtcpInterleavedChannel);
            
            return "RTP/AVP/TCP;unicast;interleaved="
            + std::string(rtp_channel)
            + "-"
            + std::string(rtcp_channel);
        }
        
        void SetRtpInterleavedChannel(int rtpInterleavedChannel) {
            m_rtpInterleavedChannel = rtpInterleavedChannel;
        }
        
        void SetRtcpInterleavedChannel(int rtcpInterleavedChannel) {
            m_rtcpInterleavedChannel = rtcpInterleavedChannel;
        }
        
    protected:
        bool IsInterleavedTransport() override {
            return true;
        }
        
        bool HandlePacket(const char* buffer, size_t size) override;
        
    private:
        bool BufferContainsRtpPacket() {
            return m_receivedBuffer.size() >= 4 && m_receivedBuffer[0] == '$';
        }
        
    private:
        int m_readBufferSize;
        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;
    };
}

#endif /* InterleavedRtspTransport_h */
