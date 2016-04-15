#include "UdpTransport.h"
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
#include <sys/ioctl.h>

#include <sstream>
  using std::ostringstream;
#include <stdexcept>
  using std::runtime_error;

using SurgeUtil::Constants::DEFAULT_RTP_PORT;
using SurgeUtil::Constants::DEFAULT_RTCP_PORT;
using SurgeUtil::Constants::DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE;


Surge::UdpTransport::UdpTransport(SocketHandlerDelegate * const delegate)
    : UdpTransport(delegate, DEFAULT_RTP_PORT, DEFAULT_RTCP_PORT) { }

Surge::UdpTransport::UdpTransport(SocketHandlerDelegate * const delegate, int rtpPort, int rtcpPort)
    : Transport(delegate),
      m_readBufferSize(DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE),
      m_rtpPort(rtpPort),
      m_rtcpPort(rtcpPort),
      m_rtpServerFD(-1),
      m_rtcpServerFD(-1)
{
    m_rtpServerFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_rtpServerFD == -1) {
        ostringstream message;
        message << "Unable to create TCP server socket";
        throw runtime_error{ message.str() };
    }

    bool non_blocking_failed = SurgeUtil::SetNonBlocking(m_rtpServerFD) == -1;
    if (non_blocking_failed) {
        close(m_rtpServerFD);
        
        ostringstream message;
        message << "Unable to set socket into non-blocking mode";
        throw runtime_error{ message.str() };
    }

    struct sockaddr_in rtp_serv_addr;
    rtp_serv_addr.sin_family = AF_INET;
    rtp_serv_addr.sin_addr.s_addr = INADDR_ANY;
    rtp_serv_addr.sin_port = htons(m_rtpPort);

    int enable = 1;
    if (setsockopt(m_rtpServerFD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        ERROR("setsockopt(SO_REUSEADDR) failed");
    
    if (bind(m_rtpServerFD, (struct sockaddr *) &rtp_serv_addr, sizeof(rtp_serv_addr)) < 0) {
        close(m_rtpServerFD);
        
        ostringstream message;
        message << "Unable to open RTP UDP server-socket on: " << m_rtpPort;
        throw runtime_error{ message.str() };
    }

    m_rtpSocketFDEvent = new SurgeUtil::BasicFDEvent(
        m_rtpServerFD,
        SurgeUtil::WaitableEvents::WatchForType::readable
        );

    m_events.push_back(m_rtpSocketFDEvent);
}

Surge::UdpTransport::~UdpTransport() {
    if (m_rtpServerFD != -1) {
        close(m_rtpServerFD);
    }

    if (m_rtcpServerFD != -1) {
        close(m_rtcpServerFD);
    }

    if (m_rtpSocketFDEvent) {
        delete m_rtpSocketFDEvent;
    }

    if (m_rtcpSocketFDEvent != nullptr) {
        delete m_rtcpSocketFDEvent;
    }
}

void Surge::UdpTransport::HandleEvent(const SurgeUtil::WaitableEvent * const event) {
    if (event == m_rtpSocketFDEvent) {
        HandleRtpSocket(event);
    }
    else {
        ERROR("Unknown event!");
    }
}

void Surge::UdpTransport::HandleRtpSocket(const SurgeUtil::WaitableEvent * const event) {
    do {
        unsigned char packet_data[256];

        sockaddr_in from;
        socklen_t fromLength = sizeof(from);

        size_t read_bytes = recvfrom(event->FD(), packet_data, sizeof(packet_data), 0,
                                     (sockaddr*)&from, &fromLength);
        if (read_bytes <= 0)
            break;

        try {
            RtpPacket* pack = new RtpPacket(packet_data, read_bytes);
            m_rtpPacketSubject.get_subscriber().on_next(pack);
        } catch (const std::exception& e) {
            ERROR("Invalid Rtp Packet: " << e.what());
        }
        
    } while (m_running && event->IsFired());
}

void Surge::UdpTransport::RtspHandleReceive(const SurgeUtil::WaitableEvent& event) {
    std::vector<unsigned char> response;
    
    if (m_receivedBuffer.size() > 0) {
        // copy trailing data into response
        response.resize(m_receivedBuffer.size());
        response = m_receivedBuffer;
        m_receivedBuffer.clear();
        m_receivedBuffer.resize(0);
    }

    size_t total_buffer_size = response.size();
    unsigned char *buffer = (unsigned char *) malloc(m_readBufferSize);
    do {
        memset((void*)buffer, 0, m_readBufferSize);
        
        size_t received = recv(event.FD(), buffer, m_readBufferSize, 0);
        if (received == -1) {
            ERROR("Failed to recv errno: " << errno);
            free(buffer);
            // Notify Delegate of Socket Failure...
            NotifyDelegateOfReadFailure();
            m_running = false;
            break;
        }
        else if (received > 0) {
            // Append received data to 'response'.
            size_t old_size = response.size();
            if (old_size < (old_size + received)) {
                response.resize(old_size + received);
            }
            copy(buffer, buffer + received, response.begin() + old_size);
            total_buffer_size += received;
        }
    } while (event.IsFired() && m_running);
    free(buffer);

    if (!m_running) {
        return;
    }
    
    size_t offs = 0;
    do {
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
