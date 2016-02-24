// -*-c++-*-
#ifndef __MUTEX_LOCKER_H__
#define __MUTEX_LOCKER_H__

// TycoUtility Headers
#include "Mutex.h"

namespace SurgeUtil {
   /**
    * RAII mutex-locker.
    */
    class Mutex_locker
    {
      public:
       /**
        * Construct, locking specified mutex.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        Mutex_locker(Mutex& a_mutex): m_mutex(a_mutex) { m_mutex.Lock(); }

       /**
        * Destruct, unlocking associated mutex.
        *
        * @throws None
        */
        ~Mutex_locker()
        {
            try
            {
                m_mutex.Unlock();
            }
            catch (...)
            {
                // Swallow this, as destructor must not throw exception.
            }
        }

      private:
       /**
        * Underlying mutex.
        */
        Mutex& m_mutex;
    };
}

#endif //__MUTEX_LOCKER_H__

