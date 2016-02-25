// -*-c++-*-
#ifndef __STOPPABLE_THREAD_H__
#define __STOPPABLE_THREAD_H__

// TycoUtility Headers
#include "FireableEvent.h"
#include "Thread.h"

namespace SurgeUtil {
   /**
    * Represents a thread that may be commanded to stop and
    * that has an associated WaitableEvent that is fired when
    * it has been commanded to stop.
    */
    class StoppableThread: public Thread
    {
      public:
       /**
        * Execute specified runnable.
        *
        * @pre    !IsRunning()
        * @pre    runnable will check the state of the
        *         StopRequested() event and voluntarily
        *         stop when this event has been fired.
        *
        * @post   This thread is executing runnable.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        void Execute(Runnable& runnable) override
        {
            m_stopRequested.Reset();
            Thread::Execute(runnable);
        }

       /**
        * Request this thread to stop running and wait for it to stop.
        *
        * @pre    None
        *
        * @post   The StopRequested() event has been fired and
        *         the current thread has waited if necessary until 
        *         this StoppableThread has stopped running.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        void Stop()
        {
            m_stopRequested.Fire();
            WaitUntilStopped();
        }

        /**
         * Fire the stop-requested event but dont join
         *
         * @pre    None
         *
         * @post   The StopRequested() event has been fired
         *
         * @throws None
         */
        void StopDontJoin()
        {
            m_stopRequested.Fire();
        }

        void Reset()
        {
            m_stopRequested.Reset();
        }

       /**
        * Stop-requested event.
        *
        * @pre    None
        *
        * @post   The stop-requested event has been returned.
        *         This is the event that when fired signals that 
        *         the thread has been requested to stop.
        *
        * @throws None
        */
        const SurgeUtil::WaitableEvent& StopRequested() const { return m_stopRequested; }

      private:
       /**
        * Firing this event requests the thread to stop.
        */
        SurgeUtil::FireableEvent m_stopRequested;
    };
}

#endif

