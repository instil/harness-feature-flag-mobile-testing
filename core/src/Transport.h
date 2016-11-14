//// -*-c++-*-
//// Copyright (c) 2016 Instil Software.
////
//// Permission is hereby granted, free of charge, to any person obtaining a copy
//// of this software and associated documentation files (the "Software"), to deal
//// in the Software without restriction, including without limitation the rights
//// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//// copies of the Software, and to permit persons to whom the Software is
//// furnished to do so, subject to the following conditions:
////
//// The above copyright notice and this permission notice shall be included in
//// all copies or substantial portions of the Software.
////
//// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//// THE SOFTWARE.
//
//#ifndef __TRANSPORT_H__
//#define __TRANSPORT_H__
//
//#include "ISocketHandlerDelegate.h"
//#include "ITransportInterface.h"
//#include "Logging.h"
//#include "Helpers.h"
//#include "FireableEvent.h"
//#include "BasicFDEvent.h"
//#include "StoppableThread.h"
//#include "RtspCommand.h"
//#include "Response.h"
//#include "RtpPacket.h"
//#include "Mutex.h"
//#include "DataEventQueue.h"
//#include "WaitableEvent.h"
//#include "Constants.h"
//
//#include <fcntl.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <unistd.h>
//#include <vector>
//
//using SurgeUtil::Constants::DEFAULT_SOCKET_HANDLER_TIMEOUT_MS;
//using SurgeUtil::Constants::DEFAULT_CONNECT_TIMEOUT_MS;
//using SurgeUtil::Constants::DEFAULT_TRANSACTION_TIMEOUT_MS;
//
//namespace Surge {
//
//    class Transport : public ITransportInterface, private SurgeUtil::Runnable {
//    public:
//        Transport(ISocketHandlerDelegate *delegate) : m_running(false),
//                                                     m_delegate(delegate),
//                                                     m_rtspSocketFD(-1),
//                                                     m_connectTimeoutMs(DEFAULT_CONNECT_TIMEOUT_MS),
//                                                     m_transactionTimeoutMs(DEFAULT_TRANSACTION_TIMEOUT_MS),
//                                                     m_timeoutMs(DEFAULT_SOCKET_HANDLER_TIMEOUT_MS) { }
//
//        virtual ~Transport() {
//            StopRunning();
//
//            m_rtspOutputQueue.Flush([&] (Response* resp) {
//                delete resp;
//            });
//        }
//
//        void StartRunning() override {
//            if (IsRunning()) {
//                return;
//            }
//
//            m_running = true;
//            m_thread.Execute(*this);
//        }
//        
//        void StopRunning() override {
//            if (!IsRunning()) {
//                return;
//            }
//
//            m_running = false;
//            m_thread.Stop();
//        }
//
//        bool IsRunning() override { return m_thread.IsRunning(); }
//
//        void SetTimeout(long timeout) override { m_timeoutMs = timeout; }
//        
//        void SetConnectTimeout(long timeout) override { m_connectTimeoutMs = timeout; }
//
//        void SetTransactionTimeout(long timeout) override { m_connectTimeoutMs = timeout; }
//
//        int RtspTcpOpen(const std::string& host, int port, const SurgeUtil::FireableEvent& abort) override {
//            DEBUG("Opening RTSP Tcp connection to: [" << host << ":" << port << "]");
//            
//            m_rtspSocketFD = socket(AF_INET, SOCK_STREAM, 0);
//            if (m_rtspSocketFD == -1) {
//                ERROR("failed to create socket: error (" << errno << ") " << strerror(errno));
//                return -1;
//            }
//
//            bool non_blocking_failed = SurgeUtil::SetNonBlocking(m_rtspSocketFD) == -1;
//            if (non_blocking_failed) {
//                ERROR("Failed to put socket into non-blocking mode: error (" << errno << ") " << strerror(errno));
//                close(m_rtspSocketFD);
//                return -1;
//            }
//
//            struct sockaddr_in serv_addr;
//            memset(&serv_addr, 0, sizeof(serv_addr));
//            serv_addr.sin_family = AF_INET;
//            serv_addr.sin_port = htons(port);
//            if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) == -1) {
//                ERROR("inet_pton failed to handle host: " << host);
//                close(m_rtspSocketFD);
//                return -1;
//            }
//    
//            if (connect(m_rtspSocketFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
//                SurgeUtil::BasicFDEvent attemptCompleted {
//                        m_rtspSocketFD,
//                        SurgeUtil::WaitableEvents::WatchForType::writable
//                };
//                
//                auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({
//                        &attemptCompleted,
//                        &abort
//                    },
//                    m_connectTimeoutMs
//                );
//                
//                if (firedEvents.empty()) {
//                    ERROR("socket connection attempt timed out");
//                    close(m_rtspSocketFD);
//                    return -1;
//                }
//                
//                if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, abort)) {
//                    INFO("socket connection attempt aborted");
//                    close(m_rtspSocketFD);
//                    return -1;
//                }
//                
//                if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, attemptCompleted))
//                {
//                    // Connection attempt completed -  Check whether successful or not.
//                    int error_code;
//                    socklen_t optionLength = sizeof(error_code);
//                    if (getsockopt(m_rtspSocketFD, SOL_SOCKET, SO_ERROR, &error_code, &optionLength) != 0) {
//                        ERROR("failed to determine result of socket connection attempt: error (" << errno << ") " << strerror(errno));
//                        close(m_rtspSocketFD);
//                        return -1;
//                    }
//                    
//                    if (error_code != 0)
//                    {
//                        ERROR("socket connection attempt failed: error code =  " << error_code);
//                        close(m_rtspSocketFD);
//                        return -1;
//                    }
//                }
//            }
//    
//            INFO("Connection to: " << host << ":" << port << " - Open");
//            
//            return 0;
//        }
//
//        Response* RtspTransaction(const RtspCommand* command, bool waitForResponse = true) override {
//            SurgeUtil::MutexLocker lock(m_mutex);
//
//            m_rtspOutputQueue.Flush([&] (Response* resp) {
//                    delete resp;
//                });
//
//            bool ok = ProcessSend(m_rtspSocketFD, command->BytesPointer(), command->PointerLength());        
//            if (!ok) {
//                ERROR("Failed to send RTSP command.");
//                return nullptr;
//            }
//    
//            Surge::Response* resp = nullptr;
//            DEBUG("Command: " << command->StringDump());
//            if (waitForResponse) {
//                auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({&m_rtspOutputQueue.GetNonEmptyEvent()},
//                                                              m_transactionTimeoutMs);
//
//                if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_rtspOutputQueue.GetNonEmptyEvent())) {
//                    resp = m_rtspOutputQueue.RemoveItem();
//                    DEBUG("TRANSACTION RESPONSE: " << resp->StringDump());
//                }
//            }
//
//            return resp;
//        }
//
//        rxcpp::observable<RtpPacket*> GetRtpPacketObservable() const override {
//            return m_rtpPacketSubject.get_observable();
//        }
//
//        void SetDelegate(ISocketHandlerDelegate * const delegate) override { m_delegate = delegate; }
//        
//    protected:
//
//        std::atomic<bool> m_running;
//
//        virtual void RtspHandleReceive(const SurgeUtil::WaitableEvent& event) = 0;
//
//        virtual void HandleEvent(const SurgeUtil::WaitableEvent * const event) { }
//
//        virtual std::vector<const SurgeUtil::BasicFDEvent*> GetWaitables() const { return {}; }
//
//        void Run() override {
//            SurgeUtil::BasicFDEvent rtsp_socket_data_available {
//                m_rtspSocketFD,
//                SurgeUtil::WaitableEvents::WatchForType::readable
//            };
//
//            while (m_running) {
//                std::vector<const SurgeUtil::WaitableEvent*> events_vector;
//                events_vector.push_back(&rtsp_socket_data_available);
//                events_vector.push_back(m_thread.StopRequested());
//                
//                for (const SurgeUtil::BasicFDEvent *event: GetWaitables()) {
//                    events_vector.push_back(event);
//                }
//                
//                auto firedEvents = SurgeUtil::WaitableEvents::WaitFor(events_vector, m_timeoutMs);
//
//                if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, *(m_thread.StopRequested()))) {
//                    DEBUG("Transport - Stop Requested.");
//                    break;
//                }
//
//                if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, rtsp_socket_data_available)) {
//                    RtspHandleReceive(rtsp_socket_data_available);
//                }
//
//                for (const SurgeUtil::WaitableEvent *event: events_vector) {
//                    if (event == &rtsp_socket_data_available || event == m_thread.StopRequested()) {
//                        continue;
//                    }
//
//                    HandleEvent(event);
//                }
//            }
//
//            close(m_rtspSocketFD);
//            INFO("SocketHandler thread finished...");
//        }
//        
//        bool ProcessSend(const int fd, const unsigned char *bytes, size_t length) {
//            return send(fd, bytes, length, 0) != -1;
//        }
//        
//        void NotifyDelegateOfReadFailure() {
//            if (m_delegate != nullptr) {
//                m_delegate->SocketReadFailed();
//            }
//        }
//
//        void NotifyDelegateOfAnnounce() {
//            if (m_delegate != nullptr) {
//                m_delegate->AnnounceReceived();
//            }
//        }
//
//        void NotifyDelegateOfRedirect() {
//            if (m_delegate != nullptr) {
//                m_delegate->RedirectReceived();
//            }
//        }
//
//        ISocketHandlerDelegate *m_delegate;
//        int m_rtspSocketFD;
//
//        long m_connectTimeoutMs;
//        long m_transactionTimeoutMs;
//        long m_timeoutMs;
//        
//        SurgeUtil::Mutex m_mutex;
//        
//        SurgeUtil::DataEventQueue<Response*> m_rtspOutputQueue;
//        rxcpp::subjects::subject<RtpPacket*> m_rtpPacketSubject;
//        
//        SurgeUtil::StoppableThread m_thread;
//    };
//    
//}
//
//#endif //__TRANSPORT_H__
