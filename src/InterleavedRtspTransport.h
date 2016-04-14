// -*-c++-*-
#ifndef __RTSP_INTERLEAVED_TRANSPORT_H__
#define __RTSP_INTERLEAVED_TRANSPORT_H__

#include "Transport.h"

#include <cstdlib>
#include <vector>


namespace Surge {
    
    class InterleavedRtspTransport : public Transport {
    public:
        InterleavedRtspTransport(SocketHandlerDelegate * const delegate);

        ~InterleavedRtspTransport();

        bool IsInterleavedTransport() override { return true; };

        bool IsRtpTransportTCP() override { return true; };

        void SetRtpInterleavedChannel(int channel) { m_rtpInterleavedChannel = channel; }

        void SetRtcpInterleavedChannel(int channel) { m_rtcpInterleavedChannel = channel; }

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

    protected:

        void RtspHandleReceive(const SurgeUtil::WaitableEvent& event) override;

    private:
        std::vector<unsigned char> m_receivedBuffer;
        size_t m_readBufferSize;
        
        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;
    };
}

#endif //__RTSP_INTERLEAVED_TRANSPORT_H__
