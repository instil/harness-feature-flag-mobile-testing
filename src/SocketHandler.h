// -*-c++-*-
#ifndef __SOCKET_HANDLER_H__
#define __SOCKET_HANDLER_H__

#include <string>

#include "StoppableThread.h"

namespace Surge {
    
    class SocketHandler : private SurgeUtil::Runnable {
    public:
        SocketHandler();

        ~SocketHandler();

        void StopRunning();

        bool IsRunning() { return m_thread.IsRunning(); }

        void SetTimeout(long timeout) { m_timeoutMs = timeout; }

        int RtspTcpOpen(const std::string host, int port);

    private:
        void Run() override;

        int m_rtspSocketFD;
        long m_timeoutMs;
        SurgeUtil::StoppableThread m_thread;
    };
}

#endif //__SOCKET_HANDLER_H__
