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

#include <algorithm>
  using std::any_of;
  using std::max;
#include <cerrno>
#include <sstream>
  using std::ostringstream;
#include <stdexcept>
  using std::runtime_error;

// System Headers
#include <sys/select.h>

#include "WaitableEvent.h"
  using SurgeUtil::WaitableEvent;
  using SurgeUtil::WaitableEvents::WatchForType;

std::vector<const WaitableEvent*> SurgeUtil::WaitableEvents::WaitFor(std::initializer_list<const WaitableEvent*> events,
                                                                     const long timeout_milliseconds)
{
    std::vector<const WaitableEvent*> events_vector;
    for (const WaitableEvent *event: events) {
        events_vector.push_back(event);
    }
    return WaitFor(events_vector, timeout_milliseconds);
}

std::vector<const WaitableEvent*> SurgeUtil::WaitableEvents::WaitFor(std::vector<const WaitableEvent*>& events,
                                                                     const long timeout_milliseconds)
{
    int max_fd {0};    // Will hold maximum file descriptor.

    fd_set read_fds;   // Will hold file descriptors waiting to become readable.
    fd_set write_fds;  // Will hold file descriptors waiting to become writable.
    fd_set exception_fds; // Will hold file descriptors waiting for "exception".

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&exception_fds);

    for (const WaitableEvent *event: events)
    {
        max_fd = max(max_fd, event->FD());

        switch(event->WatchFor())
        {
          case WatchForType::readable:
            FD_SET(event->FD(), &read_fds);
            break;

          case WatchForType::writable:
            FD_SET(event->FD(), &write_fds);
            break;

          case WatchForType::exception:
            FD_SET(event->FD(), &exception_fds);
            break;
        }
    }

    struct timeval timeout;
    timeout.tv_sec = timeout_milliseconds / 1000;
    timeout.tv_usec = (timeout_milliseconds % 1000) * 1000;

    int fired_count = -1;
    // this looks wierd but we want to always retry on EINTR
    do {
        fired_count = select(max_fd + 1,
                             &read_fds,
                             &write_fds,
                             &exception_fds,
                             &timeout);
    } while (fired_count == -1 && errno == EINTR);

    // If 'select()' did not fail then 'fired_count' is
    // the number of events that have "fired".
    // Otherwise 'fired_count' is -1.

    // Ignore select error #4 (EINTR) which seems to be prevalent on Android devices
    if (fired_count == -1)
    {
        ostringstream message;
        message << "WaitableEvents::Waitfor(): select failed: "
                << "error (" << errno << ") " << strerror(errno);
        throw runtime_error{message.str()};
    }

    std::vector<const WaitableEvent*> results;
    if (1 <= fired_count)
    {
        // Add to 'results' all events that have been determined 
        // by 'select()' to have fired.
        for (const WaitableEvent *event: events)
        {
            switch(event->WatchFor())
            {
              case WatchForType::readable:
                if (FD_ISSET(event->FD(), &read_fds))
                {
                    results.push_back(event);
                }
                break;

              case WatchForType::writable:
                if (FD_ISSET(event->FD(), &write_fds))
                {
                    results.push_back(event);
                }
                break;

              case WatchForType::exception:
                if (FD_ISSET(event->FD(), &exception_fds))
                {
                    results.push_back(event);
                }
                break;
            }
        }
    }

    return results;
}

bool SurgeUtil::WaitableEvents::IsContainedIn(const std::vector<const WaitableEvent*>& list,
                                              const WaitableEvent& event)
{
    return any_of(list.cbegin(),
                  list.cend(),
                  [&] (const WaitableEvent* e) { return e == &event; });
}

