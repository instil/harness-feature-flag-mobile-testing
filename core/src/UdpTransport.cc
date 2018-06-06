// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "UdpTransport.h"
#include "Logging.h"

Surge::UdpTransport::UdpTransport(ISocketHandlerDelegate *delegate) : Surge::Transport(delegate) {
    m_rtpClientPort = GetRandomRtpPort();
}

Surge::UdpTransport::~UdpTransport() {
    
    if (m_udp != nullptr) {
        m_udp->close();
    }
}

void Surge::UdpTransport::StopRunning() {
    if (m_udp != nullptr) {
        m_udp->close();
        m_udp = nullptr;
    }

    Surge::Transport::StopRunning();
}

void Surge::UdpTransport::RtspTcpOpen(const std::string& host, int port, std::function<void(int)>callback) {
    Transport::RtspTcpOpen(host, port, callback);
    
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
    m_udp->bind("0.0.0.0", m_rtpClientPort);
    m_udp->recv();
}

bool Surge::UdpTransport::HandleRtpPacket(const char* data, size_t size) {
    if (rtpCallback != nullptr) {
        RtpPacket* pack = new RtpPacket((unsigned char*)data, size);
        rtpCallback(pack);
    }
    
    return true;
}

void Surge::UdpTransport::PunchHoleInNat(int port) {
    DEBUG("Punching hole in NAT to open the UDP connection on port " << port << " for IP " << m_streamIp << ".");
    m_udp->send(m_streamIp, port, "HelloFromSurge", 14);
}
