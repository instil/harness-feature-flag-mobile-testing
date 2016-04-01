// -*-c++-*-
#ifndef __SOCKET_HANDLER_H__
#define __SOCKET_HANDLER_H__

#include "SocketHandlerDelegate.h"
#include "StoppableThread.h"
#include "RtspCommand.h"
#include "Response.h"
#include "RtpPacket.h"

#include "Mutex.h"
#include "DataEventQueue.h"

#include <string>

namespace Surge {
    
    class SocketHandler : private SurgeUtil::Runnable {
    public:
        SocketHandler(SocketHandlerDelegate * delegate);

        ~SocketHandler();

        void StartRunning();
        
        void StopRunning();

        bool IsRunning() { return m_thread.IsRunning(); }

        void SetTimeout(long timeout) { m_timeoutMs = timeout; }

        int RtspTcpOpen(const std::string& host, int port, const SurgeUtil::FireableEvent& abort);

        Response* RtspTransaction(const RtspCommand* command, bool waitForResponse = true);

        SurgeUtil::DataEventQueue<RtpPacket*>* GetRtpPacketQueue() { return &m_rtpOutputQueue; }

        void SetRtpInterleavedChannel(int channel) { m_rtpInterleavedChannel = channel; }

        void SetRtcpInterleavedChannel(int channel) { m_rtcpInterleavedChannel = channel; }

        void SetConnectTimeout(long timeout) { m_connectTimeoutMs = timeout; }

        void SetTransactionTimeout(long timeout) { m_connectTimeoutMs = timeout; }

    private:
        
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
        
        bool ProcessSend(const int fd, const unsigned char *bytes, size_t length);

        void HandleReceive(const SurgeUtil::WaitableEvent& event);

        void WaitForSendEventToBeHandled();
        
        void Run() override;

        SocketHandlerDelegate *m_delegate;
        
        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;

        std::vector<unsigned char> m_receivedBuffer;
        
        SurgeUtil::Mutex m_mutex;
        SurgeUtil::FireableEvent m_receivedSendEvent;
        SurgeUtil::DataEventQueue<const RtspCommand*> m_rtspInputQueue;
        SurgeUtil::DataEventQueue<Response*> m_rtspOutputQueue;
        SurgeUtil::DataEventQueue<RtpPacket*> m_rtpOutputQueue;

        std::atomic<bool> m_running;
        
        int m_rtspSocketFD;
        size_t m_readBufferSize;
        long m_connectTimeoutMs;
        long m_transactionTimeoutMs;
        long m_timeoutMs;
        SurgeUtil::StoppableThread m_thread;
    };
}

#endif //__SOCKET_HANDLER_H__
