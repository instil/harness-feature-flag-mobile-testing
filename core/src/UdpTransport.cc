// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "UdpTransport.h"
#include "Constants.h"
#include "Logging.h"


Surge::UdpTransport::UdpTransport(ISocketHandlerDelegate *delegate) : Surge::Transport(delegate) {
    INFO("");
}

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

/**************/



void Surge::UdpTransport::RtspHandleReceive(const char* buffer, size_t size) {
    if (!m_running) {
        return;
    }
    
    AppendDataToBuffer(buffer, size);
    
    do {
        if (BufferContainsAnnouncePacket()) {
            NotifyDelegateOfAnnounce();
            break;
        }
        else if (BufferContainsRedirectPacket()) {
            NotifyDelegateOfRedirect();
            break;
        }
        else if (BufferContainsRtspPacket()) {
            bool result = HandleRtspPacket();
            if (!result) {
                break;
            }
        }
        else {
            break;
        }
    } while (0 < m_receivedBuffer.size() && m_running);
}

bool Surge::UdpTransport::HandleRtpPacket(const char* data, size_t size) {
    if (rtpCallback != nullptr) {
        RtpPacket* pack = new RtpPacket((unsigned char*)data, size);
        rtpCallback(pack);
    }
    
    return true;
}


bool Surge::UdpTransport::HandleRtspPacket() {
    std::string string_response;
    string_response.resize(m_receivedBuffer.size());
    std::copy(m_receivedBuffer.begin(), m_receivedBuffer.end(), string_response.begin());
    
    size_t body_pos;
    if ((body_pos = string_response.find("\r\n\r\n")) != std::string::npos) {
        size_t pos = string_response.find("Content-Length:");
        size_t content_length = (pos != std::string::npos) ?
        static_cast<size_t>(atoi(&string_response[pos + 16])) :
        0;
        
        size_t headers_length = body_pos;
        
        const unsigned char *rtsp_buffer = &(m_receivedBuffer[0]);
        size_t rtsp_buffer_length = content_length + 4 + headers_length;
        
        if (m_receivedBuffer.size() < rtsp_buffer_length) {
            return false;
        }
        
        if (rtspCallback != nullptr) {
            rtspCallback(new Response(rtsp_buffer, rtsp_buffer_length));
            rtspCallback = nullptr;
        }
        
        RemoveDataFromStartOfBuffer(rtsp_buffer_length);
    } else {
        return false;
    }
    
    return true;
}

void Surge::UdpTransport::AppendDataToBuffer(const char* buffer, size_t size) {
    if (size > 0) {
        // Append received data to 'response'.
        size_t old_size = m_receivedBuffer.size();
        if (old_size < (old_size + size)) {
            m_receivedBuffer.resize(old_size + size);
        }
        copy(buffer, buffer + size, m_receivedBuffer.begin() + old_size);
    }
}

void Surge::UdpTransport::RemoveDataFromStartOfBuffer(size_t count) {
    m_receivedBuffer.erase(m_receivedBuffer.begin(), m_receivedBuffer.begin() + count);
}

