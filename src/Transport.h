// -*-c++-*-
#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include "SocketHandlerDelegate.h"
#include "ITransportInterface.h"
#include "Logging.h"
#include "Helpers.h"
#include "FireableEvent.h"
#include "BasicFDEvent.h"
#include "StoppableThread.h"
#include "RtspCommand.h"
#include "Response.h"
#include "RtpPacket.h"
#include "Mutex.h"
#include "DataEventQueue.h"
#include "Constants.h"

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

using SurgeUtil::Constants::DEFAULT_SOCKET_HANDLER_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_CONNECT_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_TRANSACTION_TIMEOUT_MS;

namespace Surge {

    class Transport : public ITransportInterface, private SurgeUtil::Runnable {
    public:
        Transport(SocketHandlerDelegate *delegate) : m_delegate(delegate),
                                                     m_rtspSocketFD(-1),
                                                     m_running(false),
                                                     m_connectTimeoutMs(DEFAULT_CONNECT_TIMEOUT_MS),
                                                     m_transactionTimeoutMs(DEFAULT_TRANSACTION_TIMEOUT_MS),
                                                     m_timeoutMs(DEFAULT_SOCKET_HANDLER_TIMEOUT_MS) { }

        virtual ~Transport() {
            StopRunning();

            m_rtspOutputQueue.Flush([&] (Response* resp) {
                delete resp;
            });
        }

        void StartRunning() override {
            if (IsRunning()) {
                return;
            }

            m_running = true;
            m_thread.Execute(*this);
        }
        
        void StopRunning() override {
            if (!IsRunning()) {
                return;
            }

            m_running = false;
            m_thread.Stop();
        }

        bool IsRunning() override { return m_thread.IsRunning(); }

        void SetTimeout(long timeout) override { m_timeoutMs = timeout; }
        
        void SetConnectTimeout(long timeout) override { m_connectTimeoutMs = timeout; }

        void SetTransactionTimeout(long timeout) override { m_connectTimeoutMs = timeout; }

        int RtspTcpOpen(const std::string& host, int port, const SurgeUtil::FireableEvent& abort) override {
            DEBUG("Opening RTSP Tcp connection to: [" << host << ":" << port << "]");
            
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
    
            if (connect(m_rtspSocketFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
                SurgeUtil::BasicFDEvent attemptCompleted {
                        m_rtspSocketFD,
                        SurgeUtil::WaitableEvents::WatchForType::writable
                };
                
                auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({
                        &attemptCompleted,
                        &abort
                    },
                    m_connectTimeoutMs
                );
                
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

        Response* RtspTransaction(const RtspCommand* command, bool waitForResponse = true) override {
            SurgeUtil::MutexLocker lock(m_mutex);

            m_rtspOutputQueue.Flush([&] (Response* resp) {
                    delete resp;
                });

            bool ok = ProcessSend(m_rtspSocketFD, command->BytesPointer(), command->PointerLength());        
            if (!ok) {
                ERROR("Failed to send RTSP command.");
                return nullptr;
            }
    
            Surge::Response* resp = nullptr;
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

        rxcpp::observable<RtpPacket*> GetRtpPacketObservable() const override {
            return m_rtpPacketSubject.get_observable();
        }

        void SetDelegate(SocketHandlerDelegate * const delegate) override { m_delegate = delegate; }
        
    protected:

        virtual void RtspHandleReceive(const SurgeUtil::WaitableEvent& event) = 0;

        void Run() override {
            SurgeUtil::BasicFDEvent rtsp_socket_data_available {
                m_rtspSocketFD,
                SurgeUtil::WaitableEvents::WatchForType::readable
            };
    
            while (m_running) {
        
                auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({
                        &m_thread.StopRequested(),
                        &rtsp_socket_data_available
                    },
                    m_timeoutMs);

                if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_thread.StopRequested())) {
                    DEBUG("Transport - Stop Requested.");
                    break;
                }

                if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, rtsp_socket_data_available)) {
                    RtspHandleReceive(rtsp_socket_data_available);
                }
            }

            close(m_rtspSocketFD);
            DEBUG("SocketHandler thread finished...");
        }
        
        bool ProcessSend(const int fd, const unsigned char *bytes, size_t length) {
            return send(fd, bytes, length, 0) != -1;
        }
        
        void NotifyDelegateOfReadFailure() {
            if (m_delegate != nullptr) {
                m_delegate->SocketReadFailed();
            }
        }

        void NotifyDelegateOfAnnounce() {
            if (m_delegate != nullptr) {
                m_delegate->AnnounceReceived();
            }
        }

        void NotifyDelegateOfRedirect() {
            if (m_delegate != nullptr) {
                m_delegate->RedirectReceived();
            }
        }

        SocketHandlerDelegate *m_delegate;
        int m_rtspSocketFD;

        std::atomic<bool> m_running;
        long m_connectTimeoutMs;
        long m_transactionTimeoutMs;
        long m_timeoutMs;
        
        SurgeUtil::Mutex m_mutex;
        
        SurgeUtil::DataEventQueue<Response*> m_rtspOutputQueue;
        rxcpp::subjects::subject<RtpPacket*> m_rtpPacketSubject;
        
        SurgeUtil::StoppableThread m_thread;
    };
    
}

#endif //__TRANSPORT_H__
