// -*-c++-*-
// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef __FIREABLE_EVENT_H__
#define __FIREABLE_EVENT_H__

#include "BaseWaitableEvent.h"
#include "Mutex.h"

namespace SurgeUtil {
   /**
    * WaitableEvent that may be fired (and reset) on request.
    */
    class FireableEvent: public BaseWaitableEvent
    {
      public:
       /**
        * Construct.
        *
        * @post   This event is in the unfired state.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        FireableEvent();

       /**
        * Destructor.
        *
        * @throws None
        */
        ~FireableEvent() override;

       /**
        * Fire this event.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        void Fire();

       /**
        * Reset this event to unfired state.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        void Reset();

       /**
        * File descriptor to watch when waiting for event.
        */
        int FD() const override { return m_pipeFD[0]; }

        /**
         * Get/Set associated data...
         **/
        void * GetAssociated() const { return m_associatedData; }

        int GetInt() const { return m_associatedInt; }

        void SetInt(int value) { m_associatedInt = value; }
        
        void SetAssociated(void *data) { m_associatedData = data; }

       /**
        * What to watch for on the file descriptor when
        * waiting for event.
        */
        WaitableEvents::WatchForType WatchFor() const override
        {
            return WaitableEvents::WatchForType::readable;
        }

      private:

        /* Hold associated data with event */
        void *m_associatedData;
        int m_associatedInt;
        
       /**
        * Holds file descriptors of pipe.
        * Element 0 is file descriptor of "read" end of pipe.
        * Element 1 is file descriptor of "write" end of pipe.
        */
        int m_pipeFD[2];

       /**
        * Mutex to lock the pipe.
        */
        Mutex m_pipeMutex;
    };
}

#endif

