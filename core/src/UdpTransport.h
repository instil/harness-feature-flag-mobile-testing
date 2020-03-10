// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef UdpTransport_h
#define UdpTransport_h

#include "Transport.h"

#include "uvw.hpp"
#include "Constants.h"
#include "Helpers.h"


namespace Surge {
    
    class UdpTransport : public Transport {
        
    public:
        UdpTransport(TransportDelegate * const transportDelegate, ISocketHandlerDelegate *delegate);
        ~UdpTransport();
        
        void StopRunning(bool waitUntilStopped = true) override;
        
        void RtspTcpOpen(SurgeUtil::Url &url, std::function<void(int)> callback) override;
        
        bool IsInterleavedTransport() override { return false; };
        
    protected:
        std::shared_ptr<uvw::UDPHandle> m_udp;
        
        /**************/
        
    public:
        std::string GetTransportHeaderString() const override {
            char rtp_port[12];
            char rtcp_port[12];

            memset(rtp_port, 0, sizeof(rtp_port));
            memset(rtcp_port, 0, sizeof(rtcp_port));
            snprintf(rtp_port, sizeof(rtp_port), "%d", m_rtpClientPort);
            snprintf(rtcp_port, sizeof(rtcp_port), "%d", m_rtpClientPort + 1);
            
            return "RTP/AVP/UDP;unicast;client_port="
            + std::string(rtp_port)
            + "-"
            + std::string(rtcp_port);
        }
        
    private:
        bool HandlePacket(const char* buffer, size_t size) override {
            return false;
        }
        
        bool HandleRtpPacket(const char* data, size_t size);

        int GetRandomRtpPort() {
            return SurgeUtil::RandomEvenNumberBetween(SurgeUtil::Constants::MIN_RTP_PORT_NUMBER,
                                                      SurgeUtil::Constants::MAX_RTP_PORT_NUMBER);
        }
        
        /**************/
        
    public:
        void SetRtpPortsAndChannels(const SetupResponse *setupResponse) override {
            m_rtpServerPort = setupResponse->GetRtpServerPort();
            m_rtcpServerPort = setupResponse->GetRtcpServerPort();
            PunchHoleInNat(m_rtpServerPort);
            PunchHoleInNat(m_rtcpServerPort);
        }
        
    private:
        void PunchHoleInNat(int port);
        
    private:
        int m_rtpClientPort;

        int m_rtpServerPort;
        int m_rtcpServerPort;
    };
}

#endif /* NewUdpTransport_h */
