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


Surge::SocketHandler::SocketHandler():
    m_rtpInterleavedChannel(DEFAULT_RTP_INTERLEAVED_CHANNEL),
    m_rtcpInterleavedChannel(DEFAULT_RTCP_INTERLEAVED_CHANNEL),
    m_rtspInputQueue(),
    m_rtspOutputQueue(),
    m_rtpOutputQueue(),
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

    m_thread.Execute(*this);
}

void Surge::SocketHandler::StopRunning() {
    if (!IsRunning()) {
        return;
    }

    m_thread.Stop();
}

int Surge::SocketHandler::RtspTcpOpen(const std::string host, int port) {
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
        ERROR("inet_pton");
        close(m_rtspSocketFD);
        return -1;
    } 

    if (connect(m_rtspSocketFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        
        SurgeUtil::BasicFDEvent attemptCompleted {
                m_rtspSocketFD,
                SurgeUtil::WaitableEvents::WatchForType::writable
        };
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({&attemptCompleted}, m_connectTimeoutMs);

        if (firedEvents.empty()) {
            ERROR("socket connection attempt timed out");
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
    
    INFO("Command: " << command->StringDump());
    if (waitForResponse) {
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({&m_rtspOutputQueue.GetNonEmptyEvent()},
                                                              m_transactionTimeoutMs);

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_rtspOutputQueue.GetNonEmptyEvent())) {
            resp = m_rtspOutputQueue.RemoveItem();
            INFO("TRANSACTION RESPONSE: " << resp->StringDump());
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
    
    while (true) {
        
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
            Response* resp = ReceiveResponse(rtsp_socket_data_available);

            if (resp != nullptr) {
                
                if (resp->IsInterleavedPacket())
                {
                    if (resp->GetInterleavedPacketChannelNumber() == m_rtpInterleavedChannel) {
                        
                        try {
                            std::vector<RtpPacket*> packets = resp->GetRtpPackets();
                            for (auto it = packets.begin(); it != packets.end(); ++it) {
                                m_rtpOutputQueue.AddItem(*it);
                            }
                        }
                        catch (const std::exception &exc) {
                            ERROR("ERROR PARSING RTP PACKET: " << exc.what());
                        }
                        
                    }
                    delete resp;
                }
                else {
                    m_rtspOutputQueue.AddItem(resp);
                }
            }
        }
        
    }
    INFO("SocketHandler thread finished...");
}

bool Surge::SocketHandler::ProcessSend(const int fd, const unsigned char *bytes, size_t length) {
    return send(fd, bytes, length, 0) != -1;
}

Surge::Response* Surge::SocketHandler::ReceiveResponse(const SurgeUtil::WaitableEvent& event) {
    std::vector<unsigned char> response;
    response.clear();
    response.reserve(m_readBufferSize);
    
    unsigned char *buffer = (unsigned char *) malloc(m_readBufferSize);
    do {
        memset((void*)buffer, 0, m_readBufferSize);
        
        size_t received = recv(event.FD(), buffer, m_readBufferSize, 0);
        if (received == -1) {
            ERROR("Failed to recv errno: " << errno);
            free(buffer);
            return nullptr;
        }
        else if (received > 0) {
            // Append received data to 'response'.
            size_t old_size = response.size();
            if (old_size < (old_size + received)) {
                response.resize(old_size + received);
            }
            copy(buffer,
                 buffer + received,
                 response.begin() + old_size);
        }        
    } while (event.IsFired());
    
    free(buffer);
    
    return new Surge::Response(&(response[0]), response.size());
}
