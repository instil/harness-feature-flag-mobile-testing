// -*-c++-*-
#ifndef __ERROR_DISPATCHER_H__
#define __ERROR_DISPATCHER_H__

#include "Mutex.h"
#include "MutexLocker.h"
#include "StoppableThread.h"

namespace Surge {

    typedef enum {
        
        LOST_CONNECTION,

        REDIRECT,

        ANNOUNCE,
        
    } ERROR_TYPE;    

    class RtspClient;
    
    class ErrorDispatcher : private SurgeUtil::Runnable
    {
    public:
        ErrorDispatcher();
        
        ~ErrorDispatcher() { StopRunning(); }

        static ErrorDispatcher& GetInstance() {
            static ErrorDispatcher dispatcher;
            return dispatcher;
        }
    
        void FailureForClient(const RtspClient * client, ERROR_TYPE type);

        bool IsRunning() { return m_thread.IsRunning(); }

        void StartRunning() {
            SurgeUtil::MutexLocker lock(m_Mutex);
            
            if (!IsRunning()) {
                m_thread.Execute(*this);
            }
        }

        void StopRunning() {
            SurgeUtil::MutexLocker lock(m_Mutex);
            
            if (IsRunning()) {
                m_thread.Stop();
            }
        }

    private:       
        void WaitForErrorReceived(const long timeout_milliseconds);
        void Run() override;
        
        long m_timeout;
        SurgeUtil::Mutex m_Mutex;
        SurgeUtil::Mutex m_firedEventLock;
        SurgeUtil::FireableEvent m_clientStopEvent;
        SurgeUtil::FireableEvent m_receivedStopEvent;
        SurgeUtil::StoppableThread m_thread;
    };
}

#endif //__ERROR_DISPATCHER_H__
