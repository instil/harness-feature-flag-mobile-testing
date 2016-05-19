// -*-c++-*-
#ifndef __THREAD_H__
#define __THREAD_H__

// Standard C++ Headers
#include <atomic>
#include <thread>

namespace SurgeUtil
{
   /**
    * Function object that may be executed by Thread.
    */
    struct Runnable
    {
        virtual void Run() = 0;
        virtual ~Runnable() {}
    };

   /**
    * Thread
    */
    class Thread
    {
      public:
       /**
        * Contruct.
        *
        * @post   !IsRunning()
        *
        * @throws None
        */
       Thread(): m_isRunning(false) {}

        /**
        * Destruct.
        *
        * @pre    !IsRunning()
        *
        * @throws None
        */
        ~Thread();

       /**
        * Execute specified runnable.
        *
        * @pre    !IsRunning()
        *
        * @post   This thread is executing runnable.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        virtual void Execute(Runnable& runnable);

       /**
        * Is this thread running?
        *
        * @pre    None
        *
        * @throws None
        */
        bool IsRunning() { return m_isRunning; }

       /**
        * Wait until this thread has stopped running.
        *
        * @pre    None
        * 
        * @post   Has returned immediately if thread is not running.
        *         Otherwise has waited until thread has stopped running.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        void WaitUntilStopped();

        /**
         * Proxy to return the Thread ID
         **/
        std::thread::id get_id()
        {
            return m_thread.get_id();
        }

        /**
         * Proxy to detach thread
         **/
        void DetachThread()
        {
            m_thread.detach();
        }

      private:
       /**
        * Ensure !m_thread.joinable()
        *
        * @pre    !m_isRunning
        *
        * @post   !m_thread.joinable()
        *
        * @throws std::system_error
        *         if there has been a failure.
        */
        void EnsureNotJoinable();

       /**
        * Underlying thread.
        */
        std::thread m_thread;

       /**
        * Is this thread running? (i.e. is m_thread executing code?).
        */
        std::atomic<bool> m_isRunning;
    };
}

#endif

