// -*-c++-*-
#ifndef __MUTEX_H__
#define __MUTEX_H__

// Standard C++ Headers
#include <mutex>

namespace SurgeUtil {
    
    class Mutex {
      public:
       /**
        * Construct.
        *
        * @post    This mutex is unlocked.
        *
        * @throws  None
        */
        Mutex() = default;

       /**
        * Lock.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        void Lock();

       /**
        * Unlock.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        void Unlock();

      private:
       /**
        * Underlying mutex.
        */
        std::mutex m_mutex;
    };
}

#endif

