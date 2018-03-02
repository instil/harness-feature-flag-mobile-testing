// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef UdpTransport_h
#define UdpTransport_h

#include "Transport.h"

#include "uvw.hpp"
#include "Constants.h"


namespace Surge {
    
    class UdpTransport : public Transport {
        
    public:
        UdpTransport(ISocketHandlerDelegate *delegate);
        ~UdpTransport();
        
        void StopRunning() override;
        
        void RtspTcpOpen(const std::string& host, int port, std::function<void(int)> callback) override;
        
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
            snprintf(rtp_port, sizeof(rtp_port), "%d", SurgeUtil::Constants::DEFAULT_RTP_PORT);
            snprintf(rtcp_port, sizeof(rtcp_port), "%d", SurgeUtil::Constants::DEFAULT_RTCP_PORT);
            
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
        
        /**************/
        
    public:
        void SetRtpServerPort(int rtpServerPort) {
            m_rtpServerPort = rtpServerPort;
            PunchHoleInNat(rtpServerPort);
        }
        
        void SetRtcpServerPort(int rtcpServerPort) {
            m_rtcpServerPort = rtcpServerPort;
            PunchHoleInNat(rtcpServerPort);
        }
        
    private:
        void PunchHoleInNat(int port);
        
    private:
        int m_rtpServerPort;
        int m_rtcpServerPort;
    };
}

#endif /* NewUdpTransport_h */
