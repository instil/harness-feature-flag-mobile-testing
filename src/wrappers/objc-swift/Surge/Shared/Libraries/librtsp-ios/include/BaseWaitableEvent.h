// -*-c++-*-
#ifndef __BASE_WAITABLE_EVENT_H__
#define __BASE_WAITABLE_EVENT_H__

#include "WaitableEvent.h"

namespace SurgeUtil {
   /**
    * Abstract base class implementing WaitableEvent.
    */
    struct BaseWaitableEvent: WaitableEvent
    {
       /**
        * Is this event in the "fired" state.
        */
        bool IsFired() const override
        {
            return !WaitableEvents::WaitFor({this}, 0).empty();
        }
    };
}

#endif

