// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

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
        ErrorDispatcher();
        
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
