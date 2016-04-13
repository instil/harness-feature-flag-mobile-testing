// -*-c++-*-
#ifndef __WAITABLE_EVENT_H__
#define __WAITABLE_EVENT_H__

#include <initializer_list>
#include <vector>

namespace SurgeUtil {
    
   /*
    * Namespace to hold definitions associated with
    * the WaitableEvent class.
    */
    namespace WaitableEvents
    {
       /**
        * Things that may be watched for on the file descriptor
        * when waiting for event.
        */
        enum class WatchForType {readable, writable, exception};
    }

   /**
    * Interface of an event that may be waited-for.
    *
    * These events are associated with a file descriptor and 
    * when the event fires then (depending on the type of event)
    * the file descriptor becomes readable or writable or
    * an "exceptional condition" is present.
    *
    * See Linux "man select_tut" for meaning of "exceptional condition".
    */
    struct WaitableEvent
    {
       /**
        * Destructor.
        *
        * @throws None
        */
        virtual ~WaitableEvent() {}

       /**
        * Is this event in the "fired" state.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        virtual bool IsFired() const = 0;

       /**
        * File descriptor to watch when waiting for event.
        *
        * @throws None
        *
        * @note   Used by WaitableEvents::WaitFor().
        */
        virtual int FD() const = 0;

       /**
        * What to watch for on the file descriptor when
        * waiting for event.
        *
        * @throws None
        *
        * @note   Used by WaitableEvents::WaitFor().
        */
        virtual WaitableEvents::WatchForType WatchFor() const = 0;
    };

   /*
    * Namespace to hold functions that operate on WaitableEvent objects.
    */
    namespace WaitableEvents
    {
       /**
        * Wait for at least one of the specified events to fire,
        * or for the specified timeout period to elapse.
        *
        * @pre    0 <= a_timeout_milliseconds
        *
        * @post   A list of pointers to the passed events that have fired
        *         has been returned.
        *
        * @throws std::runtime_error
        *         if there has been a failure.
        */
        std::vector<const WaitableEvent*> WaitFor(std::initializer_list<const WaitableEvent*> events,
                                                  const long timeout_milliseconds);

        std::vector<const WaitableEvent*> WaitFor(std::vector<const WaitableEvent*>& events,
                                                  const long timeout_milliseconds);

       /**
        * Is the specified event contained in the specified list?
        *
        * @throws None
        */
        bool IsContainedIn(const std::vector<const WaitableEvent*>& list,
                           const WaitableEvent& event);
    }
}

#endif

