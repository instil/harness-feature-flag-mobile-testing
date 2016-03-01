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

        void StopRunning();

        bool IsRunning() { return m_thread.IsRunning(); }

        void SetTimeout(long timeout) { m_timeoutMs = timeout; }

        int RtspTcpOpen(const std::string host, int port);

        Response* RtspTransaction(const RtspCommand* command, bool waitForResponse = true);

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
