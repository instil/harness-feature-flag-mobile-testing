// -*-c++-*-
#ifndef __SOCKET_HANDLER_H__
#define __SOCKET_HANDLER_H__

#include "StoppableThread.h"
#include "RtspCommand.h"
#include "Response.h"

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

        bool ProcessSend(const int fd, const unsigned char *bytes, size_t length);

        Response* ReceiveResponse(const SurgeUtil::WaitableEvent& event);

    private:
        void Run() override;

        SurgeUtil::DataEventQueue<const RtspCommand*> m_rtspInputQueue;
        SurgeUtil::DataEventQueue<Response*> m_rtspOutputQueue;

        SurgeUtil::Mutex m_mutex;
        int m_rtspSocketFD;
        long m_timeoutMs;
        SurgeUtil::StoppableThread m_thread;
    };
}

#endif //__SOCKET_HANDLER_H__
