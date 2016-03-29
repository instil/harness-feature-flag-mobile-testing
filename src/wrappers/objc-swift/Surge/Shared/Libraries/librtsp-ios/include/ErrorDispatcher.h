// -*-c++-*-
#ifndef __ERROR_DISPATCHER_H__
#define __ERROR_DISPATCHER_H__

#include "Mutex.h"
#include "StoppableThread.h"

namespace Surge {

    typedef enum {
        
        LOST_CONNECTION
        
    } ERROR_TYPE;    

    class RtspClient;
    
    class ErrorDispatcher : private SurgeUtil::Runnable
    {
    public:
        ErrorDispatcher();
        
        ~ErrorDispatcher() { StopRunning(); }
    
        void FailureForClient(const RtspClient * client, ERROR_TYPE type);

        bool IsRunning() { return m_thread.IsRunning(); }

        void StartRunning() {
            if (!IsRunning()) {
                m_thread.Execute(*this);
            }
        }

        void StopRunning() {
            if (IsRunning()) {
                m_thread.Stop();
            }
        }

    private:
               
        void WaitForErrorReceived(const long timeout_milliseconds);
        void Run() override;
        
        long m_timeout;
        SurgeUtil::Mutex m_firedEventLock;
        SurgeUtil::FireableEvent m_clientStopEvent;
        SurgeUtil::FireableEvent m_receivedStopEvent;
        SurgeUtil::StoppableThread m_thread;
    };

    static ErrorDispatcher *__dispatcher = nullptr;
}

#endif //__ERROR_DISPATCHER_H__
