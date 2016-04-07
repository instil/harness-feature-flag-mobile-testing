#include "SocketHandler.h"
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

#include <vector>

using SurgeUtil::Constants::DEFAULT_SOCKET_HANDLER_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_RTP_INTERLEAVED_CHANNEL;
using SurgeUtil::Constants::DEFAULT_RTCP_INTERLEAVED_CHANNEL;
using SurgeUtil::Constants::DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE;
using SurgeUtil::Constants::DEFAULT_NO_PACKET_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_CONNECT_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_TRANSACTION_TIMEOUT_MS;


Surge::SocketHandler::SocketHandler(SocketHandlerDelegate * delegate):
    m_delegate(delegate),
    m_rtpInterleavedChannel(DEFAULT_RTP_INTERLEAVED_CHANNEL),
    m_rtcpInterleavedChannel(DEFAULT_RTCP_INTERLEAVED_CHANNEL),
    m_rtspInputQueue(),
    m_rtspOutputQueue(),
    m_rtpOutputQueue(),
    m_running(false),
    m_rtspSocketFD(-1),
    m_readBufferSize(DEFAULT_SOCKET_HANDLER_READ_BUFFER_SIZE),
    m_connectTimeoutMs(DEFAULT_CONNECT_TIMEOUT_MS),
    m_transactionTimeoutMs(DEFAULT_TRANSACTION_TIMEOUT_MS),
    m_timeoutMs(DEFAULT_SOCKET_HANDLER_TIMEOUT_MS)
{
    
}

Surge::SocketHandler::~SocketHandler() {
    StopRunning();
    
    m_rtspOutputQueue.Flush([&] (Response* resp) {
            delete resp;
        });
    
    m_rtpOutputQueue.Flush([&] (RtpPacket* pack) {
            delete pack;
        });
}

void Surge::SocketHandler::StartRunning() {
    if (IsRunning()) {
        return;
    }

    m_running = true;

    m_thread.Execute(*this);
}

void Surge::SocketHandler::StopRunning() {
    if (!IsRunning()) {
        return;
    }

    m_running = false;

    m_thread.Stop();
}

int Surge::SocketHandler::RtspTcpOpen(const std::string& host, int port, const SurgeUtil::FireableEvent& abort) {
    m_rtspSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_rtspSocketFD == -1) {
        ERROR("failed to create socket: error code = " << errno);
        return -1;
    }

    bool non_blocking_failed = SurgeUtil::SetNonBlocking(m_rtspSocketFD) == -1;
    if (non_blocking_failed) {
        ERROR("Failed to put socket into non-blocking mode: error code = " << errno);
        close(m_rtspSocketFD);
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) == -1) {
        ERROR("inet_pton failed to handle host: " << host);
        close(m_rtspSocketFD);
        return -1;
    }

    int socketSize  = (1 * 1024 * 1024); // 1Mb; 
    setsockopt(m_rtspSocketFD, SOL_SOCKET, SO_RCVBUF, &socketSize, sizeof(socketSize));

    int flag = 1;
    setsockopt(m_rtspSocketFD, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    
    if (connect(m_rtspSocketFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        
        SurgeUtil::BasicFDEvent attemptCompleted {
                m_rtspSocketFD,
                SurgeUtil::WaitableEvents::WatchForType::writable
        };
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({
                &attemptCompleted,
                &abort
            },
            m_connectTimeoutMs);

        if (firedEvents.empty()) {
            ERROR("socket connection attempt timed out");
            close(m_rtspSocketFD);
            return -1;
        }

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, abort)) {
            INFO("socket connection attempt aborted");
            close(m_rtspSocketFD);
            return -1;
        }

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, attemptCompleted))
        {
            // Connection attempt completed -  Check whether successful or not.
            int error_code;
            socklen_t optionLength = sizeof(error_code);
            if (getsockopt(m_rtspSocketFD, SOL_SOCKET, SO_ERROR, &error_code, &optionLength) != 0) {
                ERROR("failed to determine result of socket connection attempt: error code = " << errno);
                close(m_rtspSocketFD);
                return -1;
            }
            
            if (error_code != 0)
            {
                ERROR("socket connection attempt failed: error code =  " << error_code);
                close(m_rtspSocketFD);
                return -1;
            }
        }
    }
    
    INFO("Connection to: " << host << ":" << port << " - Open");
    
    return 0;
}

Surge::Response* Surge::SocketHandler::RtspTransaction(const RtspCommand* command, bool waitForResponse) {
    SurgeUtil::MutexLocker lock(m_mutex);

    m_receivedSendEvent.Reset();
    m_rtspOutputQueue.Flush([&] (Response* resp) {
            delete resp;
        });
    
    Surge::Response* resp = nullptr;
    m_rtspInputQueue.AddItem(command);

    WaitForSendEventToBeHandled();
    
    TRACE("Command: " << command->StringDump());
    if (waitForResponse) {
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({&m_rtspOutputQueue.GetNonEmptyEvent()},
                                                              m_transactionTimeoutMs);

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_rtspOutputQueue.GetNonEmptyEvent())) {
            resp = m_rtspOutputQueue.RemoveItem();
            TRACE("TRANSACTION RESPONSE: " << resp->StringDump());
        }
    }

    return resp;
}

void Surge::SocketHandler::WaitForSendEventToBeHandled() {
    SurgeUtil::WaitableEvents::WaitFor({&m_receivedSendEvent}, m_transactionTimeoutMs);
}

void Surge::SocketHandler::Run() {

    SurgeUtil::BasicFDEvent rtsp_socket_data_available {
            m_rtspSocketFD,
            SurgeUtil::WaitableEvents::WatchForType::readable
    };
    
    while (m_running) {
        
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({
                    &m_thread.StopRequested(),
                    &m_rtspInputQueue.GetNonEmptyEvent(),
                    &rtsp_socket_data_available
            },
            m_timeoutMs);

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_thread.StopRequested())) {
            DEBUG("SocketHandler - Stop Requested.");
            break;
        }

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_rtspInputQueue.GetNonEmptyEvent())) {
            const RtspCommand* command = m_rtspInputQueue.RemoveItem();
            bool ok = ProcessSend(m_rtspSocketFD, command->BytesPointer(), command->PointerLength());
            
            m_receivedSendEvent.Fire();
            
            if (!ok) {
                ERROR("Failed to send RTSP command.");
            }
        }

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, rtsp_socket_data_available)) {
            HandleReceive(rtsp_socket_data_available);
        }
    }

    close(m_rtspSocketFD);
    
    DEBUG("SocketHandler thread finished...");
}

bool Surge::SocketHandler::ProcessSend(const int fd, const unsigned char *bytes, size_t length) {
    return send(fd, bytes, length, 0) != -1;
}

/**
 * Interleaved data is annoying.
 *
 *  XXXYYYYYYYXXXXYZZZZZZZ
 * | read-size |
 *
 * Because we read in chunks trailing packet data must be held onto to ensure RTP/RTSP/RTCP consistency.
 **/
void Surge::SocketHandler::HandleReceive(const SurgeUtil::WaitableEvent& event) {
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
            size_t packet_size = static_cast<size_t>(packet_length);

            bool have_whole_packet = (total_buffer_size - (offs + 4)) >= packet_length;

            if (have_whole_packet && channel == m_rtpInterleavedChannel) {
                m_rtpOutputQueue.AddItem(new RtpPacket(&(response[offs + 4]), packet_size));
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
