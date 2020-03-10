// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "UdpTransport.h"
#include "Logging.h"

Surge::UdpTransport::UdpTransport(TransportDelegate * const transportDelegate, ISocketHandlerDelegate *delegate) : Surge::Transport(transportDelegate, delegate) {
    m_rtpClientPort = GetRandomRtpPort();
}

Surge::UdpTransport::~UdpTransport() {
    
    if (m_udp != nullptr) {
        m_udp->close();
    }
}

void Surge::UdpTransport::StopRunning(bool waitUntilStopped) {
    if (m_udp != nullptr) {
        m_udp->close();
        m_udp = nullptr;
    }

    Surge::Transport::StopRunning(waitUntilStopped);
}

void Surge::UdpTransport::RtspTcpOpen(SurgeUtil::Url &url, std::function<void(int)>callback) {
    Transport::RtspTcpOpen(url, callback);
    
    if (m_udp != nullptr) {
        m_udp->close();
        m_udp = nullptr;
    }
    
    m_udp = m_loop->resource<uvw::UDPHandle>();

    m_udp->on<uvw::UDPDataEvent>([this](const uvw::UDPDataEvent &dataEvent, uvw::UDPHandle &udp) {
        HandleRtpPacket(dataEvent.data.get(), dataEvent.length);
    });

    m_udp->on<uvw::ErrorEvent>([this](const uvw::ErrorEvent &error, uvw::UDPHandle &tcp) {

        if (error.code() == UV_EADDRINUSE) {
            DEBUG("Port clash - port must already be in use. Trying another port.");

            m_rtpClientPort = GetRandomRtpPort();
            m_udp->bind("0.0.0.0", m_rtpClientPort);
            m_udp->recv();
        }
    });

    DEBUG("Binding to port " << m_rtpClientPort << " for UDP RTP data.");
    if (SurgeUtil::Url::IpIsIPv6(m_streamIp)) {
        m_udp->bind<uvw::IPv6>("::0", m_rtpClientPort);
    } else {
        m_udp->bind<uvw::IPv4>("0.0.0.0", m_rtpClientPort);
    }

    m_udp->recv();
}

bool Surge::UdpTransport::HandleRtpPacket(const char* data, size_t size) {
    if (transportDelegate != nullptr) {
        RtpPacket* pack = new RtpPacket((unsigned char*)data, size);
        transportDelegate->RtpPacketReceived(pack);
    }

    return true;
}

void Surge::UdpTransport::PunchHoleInNat(int port) {
    DEBUG("Punching hole in NAT to open the UDP connection on port " << port << " for IP " << m_streamIp << ".");
    m_udp->send(m_streamIp, port, "HelloFromSurge", 14);
}
