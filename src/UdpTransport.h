// -*-c++-*-
#ifndef __TCP_TRANSPORT_H__
#define __TCP_TRANSPORT_H__

#include "Transport.h"
#include "BasicFDEvent.h"

#include <cstdlib>
#include <vector>


namespace Surge {
    
    class UdpTransport : public Transport {
    public:
        UdpTransport(SocketHandlerDelegate * const delegate);

        UdpTransport(SocketHandlerDelegate * const delegate, int rtpPort, int rtcpPort);

        ~UdpTransport();

        bool IsInterleavedTransport() override { return false; };

        bool IsRtpTransportTCP() override { return true; };

        int GetRtpTransportPort() const { return m_rtpPort; }

        int GetRtcpTransportPort() const { return m_rtcpPort; }

        void SetServerPort1(int port) { m_serverPort1 = port; }

        void SetServerPort2(int port) { m_serverPort2 = port; }

        std::string GetTransportHeaderString() const override {
            char rtp_port[12];
            char rtcp_port[12];

            memset(rtp_port, 0, sizeof(rtp_port));
            memset(rtcp_port, 0, sizeof(rtcp_port));
            snprintf(rtp_port, sizeof(rtp_port), "%d", GetRtpTransportPort());
            snprintf(rtcp_port, sizeof(rtcp_port), "%d", GetRtcpTransportPort());
            
            return "RTP/AVP/UDP;unicast;client_port="
                + std::string(rtp_port)
                + "-"
                + std::string(rtcp_port);
        }

    protected:

        void RtspHandleReceive(const SurgeUtil::WaitableEvent& event) override;

        void HandleEvent(const SurgeUtil::WaitableEvent * const event) override;

        std::vector<const SurgeUtil::BasicFDEvent*> GetWaitables() const override { return m_events; }

    private:

        void HandleRtpSocket(const SurgeUtil::WaitableEvent * const event);
        
        std::vector<unsigned char> m_receivedBuffer;
        size_t m_readBufferSize;
        
        int m_rtpPort;
        int m_rtcpPort;

        int m_serverPort1;
        int m_serverPort2;

        int m_rtpServerFD;
        int m_rtcpServerFD;

        std::vector<const SurgeUtil::BasicFDEvent*> m_events;
        
        SurgeUtil::BasicFDEvent *m_rtpSocketFDEvent;
        SurgeUtil::BasicFDEvent *m_rtcpSocketFDEvent;
    };
}

#endif //__TCP_TRANSPORT_H__
