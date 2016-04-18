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

#ifndef __BASIC_FD_EVENT_H__
#define __BASIC_FD_EVENT_H__

#include "BaseWaitableEvent.h"

namespace SurgeUtil {
   /**
    * WaitableEvent corresponding to a file descriptor being readable,
    * writable or having an "exception condition".
    */
    class BasicFDEvent: public BaseWaitableEvent
    {
      public:
       /**
        * Construct for specified file descriptor and watch-for type.
        */
        BasicFDEvent(int a_FD, WaitableEvents::WatchForType a_watchFor):
          m_FD(a_FD),
          m_watchFor(a_watchFor)
        {}

       /**
        * File descriptor to watch when waiting for event.
        */
        int FD() const override { return m_FD; }

       /**
        * What to watch for on the file descriptor when
        * waiting for event.
        */
        WaitableEvents::WatchForType WatchFor() const override
        {
            return m_watchFor;
        }

      private:
       /**
        * Holds file descriptor to monitor.
        */
        const int m_FD;

       /**
        * Holds what to watch for on file descriptor.
        */
        const WaitableEvents::WatchForType m_watchFor;
    };
}

#endif

