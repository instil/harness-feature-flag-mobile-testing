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
    UvwTransport(delegate),
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
    std::vector<unsigned char> response;
    
    /* LIBUV CHANGES */

    if (m_receivedBuffer.size() > 0) {
        // copy trailing data into response
        response.resize(m_receivedBuffer.size());
        response = m_receivedBuffer;
        m_receivedBuffer.clear();
        m_receivedBuffer.resize(0);
    }

    size_t total_buffer_size = response.size();
//    unsigned char *buffer = (unsigned char *) malloc(m_readBufferSize);
//    do {
//        memset((void*)buffer, 0, m_readBufferSize);
//        
//        size_t received = recv(event.FD(), buffer, m_readBufferSize, 0);
//        if (received == -1) {
//            int errorNo = errno;
//            ERROR("Failed to recv: error (" << errorNo << ") " << strerror(errorNo));
//            // Notify Delegate of Socket Failure...
//            NotifyDelegateOfReadFailure();
//            m_running = false;
//            break;
//        }
//        else if (received > 0) {
//            // Append received data to 'response'.
//            size_t old_size = response.size();
//            if (old_size < (old_size + received)) {
//                response.resize(old_size + received);
//            }
//            copy(buffer, buffer + received, response.begin() + old_size);
//            total_buffer_size += received;
//        }
//    } while (event.IsFired() && m_running);
//    free(buffer);
    
    if (size > 0) {
        // Append received data to 'response'.
        size_t old_size = response.size();
        if (old_size < (old_size + size)) {
            response.resize(old_size + size);
        }
        copy(buffer, buffer + size, response.begin() + old_size);
        total_buffer_size += size;
    }

    /* ~LIBUV CHANGES */

    
    if (!m_running) {
        return;
    }
    
    size_t offs = 0;
    do {
        bool is_rtp = response.size() >= 4 && response[offs] == '$';
        bool is_rtsp = response.size() >= 8 && strncmp((char*)&(response[offs]), "RTSP/1.0", 8) == 0;
        bool is_announce = response.size() >= 8 && strncmp((char*)&(response[offs]), "ANNOUNCE", 8) == 0;
        bool is_redirect = response.size() >= 8 && strncmp((char*)&(response[offs]), "REDIRECT", 8) == 0;

        if (is_announce) {
            NotifyDelegateOfAnnounce();
            break;
        }
        else if (is_redirect) {
            NotifyDelegateOfRedirect();
            break;
        }
        else if (is_rtp) {
            int channel = static_cast<int>(response[offs + 1]);

            uint16_t network_order_packet_length = 0;
            memcpy(&network_order_packet_length, &(response[offs + 2]), 2);
            uint16_t packet_length = ntohs(network_order_packet_length);

            bool have_whole_packet = (total_buffer_size - (offs + 4)) >= packet_length;

            if (have_whole_packet && channel == m_rtpInterleavedChannel) {
                try {
                    RtpPacket* pack = new RtpPacket(&(response[offs + 4]), packet_length);
                    m_rtpPacketSubject.get_subscriber().on_next(pack);
                } catch (const std::exception& e) {
                    ERROR("Invalid Rtp Packet: " << e.what());
                }
            } else if (!have_whole_packet) {
                // copy into received buffer
                size_t trailing_length = total_buffer_size - offs;
                m_receivedBuffer.resize(trailing_length);
                std::copy(response.begin() + offs, response.end(), m_receivedBuffer.begin());
                break;
            }
            offs += packet_length + 4;
        }
        else if (is_rtsp) {
            std::string string_response;
            string_response.resize(total_buffer_size - offs);
            std::copy(response.begin() + offs, response.end(), string_response.begin());

            size_t body_pos; 
            if ((body_pos = string_response.find("\r\n\r\n")) != std::string::npos) {
                size_t pos = string_response.find("Content-Length:");
                size_t content_length = (pos != std::string::npos) ?
                    static_cast<size_t>(atoi(&string_response[pos + 16])) :
                    0;
                
                size_t headers_length = body_pos;
                
                const unsigned char *rtsp_buffer = &(response[offs]);
                size_t rtsp_buffer_length = content_length + 4 + headers_length;

                if ((total_buffer_size - offs) < rtsp_buffer_length) {
                    size_t trailing_length = total_buffer_size - offs;
                    m_receivedBuffer.resize(trailing_length);
                    std::copy(response.begin() + offs, response.end(), m_receivedBuffer.begin());
                    break;
                } 
                
                m_rtspOutputQueue.AddItem(new Response(rtsp_buffer, rtsp_buffer_length));
                offs += rtsp_buffer_length;
            } else {
                size_t trailing_length = total_buffer_size - offs;
                m_receivedBuffer.resize(trailing_length);
                std::copy(response.begin() + offs, response.end(), m_receivedBuffer.begin());
                break;
            }
        }
        else {
            size_t trailing_length = total_buffer_size - offs;
            m_receivedBuffer.resize(trailing_length);
            std::copy(response.begin() + offs, response.end(), m_receivedBuffer.begin());
            break;
        }
    } while (offs < total_buffer_size && m_running);
}
