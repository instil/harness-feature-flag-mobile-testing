// -*-c++-*-
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

