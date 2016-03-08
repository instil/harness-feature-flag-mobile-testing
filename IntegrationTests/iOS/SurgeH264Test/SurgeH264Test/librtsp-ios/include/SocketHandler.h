// -*-c++-*-
#ifndef __SOCKET_HANDLER_H__
#define __SOCKET_HANDLER_H__

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
        SocketHandler();

        ~SocketHandler();

        void StartRunning();
        
        void StopRunning();

        bool IsRunning() { return m_thread.IsRunning(); }

        void SetTimeout(long timeout) { m_timeoutMs = timeout; }

        int RtspTcpOpen(const std::string host, int port);

        Response* RtspTransaction(const RtspCommand* command, bool waitForResponse = true);

        SurgeUtil::DataEventQueue<RtpPacket*>* GetRtpPacketQueue() { return &m_rtpOutputQueue; }

    private:
        bool ProcessSend(const int fd, const unsigned char *bytes, size_t length);

        Response* ReceiveResponse(const SurgeUtil::WaitableEvent& event);

        void WaitForSendEventToBeHandled();
        
        void Run() override;

        SurgeUtil::Mutex m_mutex;
        SurgeUtil::FireableEvent m_receivedSendEvent;
        SurgeUtil::DataEventQueue<const RtspCommand*> m_rtspInputQueue;
        SurgeUtil::DataEventQueue<Response*> m_rtspOutputQueue;
        SurgeUtil::DataEventQueue<RtpPacket*> m_rtpOutputQueue;

        int m_rtspSocketFD;
        long m_timeoutMs;
        SurgeUtil::StoppableThread m_thread;
    };
}

#endif //__SOCKET_HANDLER_H__
