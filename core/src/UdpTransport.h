// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef UdpTransport_h
#define UdpTransport_h

#include "Transport.h"

#include "uvw.hpp"


namespace Surge {
    
    class UdpTransport : public Transport {
    
    public:
        UdpTransport(ISocketHandlerDelegate *delegate);
        ~UdpTransport();
        
        void RtspTcpOpen(const std::string& host, int port, std::function<void(int)> callback) override;
        
        bool IsInterleavedTransport() override { return false; };

        bool IsRtpTransportTCP() override { return true; };
        
    protected:
        std::shared_ptr<uvw::UDPHandle> m_udp;
        
        /**************/
        
    public:
        void RtspHandleReceive(const char* buffer, size_t size) override;

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
        
        bool BufferContainsRtspPacket() {
            return m_receivedBuffer.size() >= 8 && strncmp((char*)&(m_receivedBuffer[0]), "RTSP/1.0", 8) == 0;
        }
        
        bool BufferContainsAnnouncePacket() {
            return m_receivedBuffer.size() >= 8 && strncmp((char*)&(m_receivedBuffer[0]), "ANNOUNCE", 8) == 0;
        }
        
        bool BufferContainsRedirectPacket() {
            return m_receivedBuffer.size() >= 8 && strncmp((char*)&(m_receivedBuffer[0]), "REDIRECT", 8) == 0;
        }
        
    private:
        bool HandleRtspPacket();
            
        bool HandleRtpPacket(const char* data, size_t size);
        void AppendDataToBuffer(const char* buffer, size_t size);
        void RemoveDataFromStartOfBuffer(size_t count);
        
        std::vector<unsigned char> m_receivedBuffer;
        size_t m_readBufferSize;
    };
}

#endif /* UdpTransport_h */
