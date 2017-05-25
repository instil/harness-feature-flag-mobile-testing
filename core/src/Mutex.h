// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __MUTEX_H__
#define __MUTEX_H__

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

