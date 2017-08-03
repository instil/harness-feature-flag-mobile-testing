// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "UdpTransport.h"
#include "Logging.h"


Surge::UdpTransport::UdpTransport(ISocketHandlerDelegate *delegate) : Surge::Transport(delegate) { }

Surge::UdpTransport::~UdpTransport() {
    
    if (m_udp != nullptr) {
        m_udp->close();
    }
}

void Surge::UdpTransport::StopRunning() {
    Surge::Transport::StopRunning();
    
    if (m_udp != nullptr) {
        m_udp->close();
        m_udp = nullptr;
    }
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
    
    m_udp->bind("0.0.0.0", SurgeUtil::Constants::DEFAULT_RTP_PORT);
    m_udp->recv();
}



bool Surge::UdpTransport::HandleRtpPacket(const char* data, size_t size) {
    if (rtpCallback != nullptr) {
        RtpPacket* pack = new RtpPacket((unsigned char*)data, size);
        rtpCallback(pack);
    }
    
    return true;
}
