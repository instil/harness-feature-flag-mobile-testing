// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "InterleavedRtspTransport.h"
#include "BasicFDEvent.h"
#include "WaitableEvent.h"
#include "MutexLocker.h"
#include "Logging.h"
#include "Helpers.h"
#include "Constants.h"

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>


using SurgeUtil::Constants::DEFAULT_RTP_INTERLEAVED_CHANNEL;
using SurgeUtil::Constants::DEFAULT_RTCP_INTERLEAVED_CHANNEL;
using SurgeUtil::Constants::DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE;


Surge::InterleavedRtspTransport::InterleavedRtspTransport(ISocketHandlerDelegate * const delegate):
    Transport(delegate),
    m_readBufferSize(DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE),
    m_rtpInterleavedChannel(DEFAULT_RTP_INTERLEAVED_CHANNEL),
    m_rtcpInterleavedChannel(DEFAULT_RTCP_INTERLEAVED_CHANNEL) { }

Surge::InterleavedRtspTransport::~InterleavedRtspTransport() { }

/**
 * Interleaved data is annoying.
 *
 *  XXXYYYYYYYXXXXYZZZZZZZ
 * | read-size |
 *
 * Because we read in chunks trailing packet data must be held onto to ensure RTP/RTSP/RTCP consistency.
 **/
void Surge::InterleavedRtspTransport::RtspHandleReceive(const char* buffer, size_t size) {
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
        else if (BufferContainsRtpPacket()) {
            bool result = HandleRtpPacket();
            if (!result) {
                break;
            }
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

bool Surge::InterleavedRtspTransport::HandleRtpPacket() {
    int channel = static_cast<int>(m_receivedBuffer[1]);
    
    uint16_t network_order_packet_length = 0;
    memcpy(&network_order_packet_length, &(m_receivedBuffer[2]), 2);
    uint16_t packet_length = ntohs(network_order_packet_length);
    
    bool have_whole_packet = (m_receivedBuffer.size() - (4)) >= packet_length;
    
    if (have_whole_packet && channel == m_rtpInterleavedChannel) {
        try {
            if (rtpCallback != nullptr) {
                RtpPacket* pack = new RtpPacket(&(m_receivedBuffer[4]), packet_length);
                rtpCallback(pack);
            }
        } catch (const std::exception& e) {
            ERROR("Invalid Rtp Packet: " << e.what());
        }
    } else if (!have_whole_packet) {
        return false;
    }
    RemoveDataFromStartOfBuffer(packet_length + 4);
    
    return true;
}

bool Surge::InterleavedRtspTransport::HandleRtspPacket() {
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
        }
            
        RemoveDataFromStartOfBuffer(rtsp_buffer_length);
    } else {
        return false;
    }
    
    return true;
}

void Surge::InterleavedRtspTransport::AppendDataToBuffer(const char* buffer, size_t size) {
    if (size > 0) {
        // Append received data to 'response'.
        size_t old_size = m_receivedBuffer.size();
        if (old_size < (old_size + size)) {
            m_receivedBuffer.resize(old_size + size);
        }
        copy(buffer, buffer + size, m_receivedBuffer.begin() + old_size);
    }
}

void Surge::InterleavedRtspTransport::RemoveDataFromStartOfBuffer(size_t count) {
    m_receivedBuffer.erase(m_receivedBuffer.begin(), m_receivedBuffer.begin() + count);
}

