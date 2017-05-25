// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __BASE_WAITABLE_EVENT_H__
#define __BASE_WAITABLE_EVENT_H__

#include "WaitableEvent.h"

namespace SurgeUtil {
   /**
    * Abstract base class implementing WaitableEvent.
    */
    class BaseWaitableEvent: public WaitableEvent {
    public:
        
       /**
        * Is this event in the "fired" state.
        */
        bool IsFired() const override {
            return !WaitableEvents::WaitFor({this}, 0).empty();
        }
    };
}

#endif

