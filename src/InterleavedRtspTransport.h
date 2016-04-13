// -*-c++-*-
#ifndef __RTSP_INTERLEAVED_TRANSPORT_H__
#define __RTSP_INTERLEAVED_TRANSPORT_H__

#include "Transport.h"

#include <vector>


namespace Surge {
    
    class InterleavedRtspTransport : public Transport {
    public:
        InterleavedRtspTransport(SocketHandlerDelegate * delegate);

        ~InterleavedRtspTransport();

        bool IsInterleavedTransport() override { return true; };

        bool IsRtpTransportTCP() override { return true; };

        void SetRtpInterleavedChannel(int channel) { m_rtpInterleavedChannel = channel; }

        void SetRtcpInterleavedChannel(int channel) { m_rtcpInterleavedChannel = channel; }

        std::string GetTransportHeaderString() const override {
            return "RTP/AVP/TCP;unicast;interleaved=0-1";
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
