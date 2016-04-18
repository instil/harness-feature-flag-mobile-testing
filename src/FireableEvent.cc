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


#include <cerrno>
#include <cstdint>
#include <sstream>
#include <stdexcept>

#include <sys/select.h>
#include <unistd.h>

#include "FireableEvent.h"
#include "MutexLocker.h"


FireableEvent::FireableEvent()
{
    if (pipe(m_pipeFD) == -1)
    {
        std::ostringstream message;
        message << "FireableEvent::FireableEvent(): failed to create pipe: "
                << "errno = " << errno;
        throw std::runtime_error{message.str()};
    }
}

FireableEvent::~FireableEvent()
{
    close(m_pipeFD[0]);
    close(m_pipeFD[1]);
}

void FireableEvent::Fire()
{
    // The pipe is in a "fired" state when it contains
    // a byte available for reading.  We avoid ever
    // having more than one byte in the pipe at a time.

    // Lock the pipe so that it does not change state
    // between checking if this event is in a "unfired" state 
    // and firing it (because we do not wish to have multiple
    // bytes in the pipe).

    SurgeUtil::MutexLocker lock{m_pipeMutex};

    if (!IsFired())
    {
        // Write one byte to pipe.
        uint8_t byte = 0xFF;
        write(m_pipeFD[1], &byte, 1);
    }
}

void FireableEvent::Reset()
{
    // We put the pipe into an "unfired" state by
    // ensuring that it contains no bytes available
    // for reading.

    // Lock the pipe so that it does not change state
    // between checking if this event is in a "fired" state 
    // and unfiring it (because we do not wish to block due
    // to reading an empty pipe).

    SurgeUtil::MutexLocker lock{m_pipeMutex};

    if (IsFired())
    {
        // Attempt to read up to 2 bytes from pipe.
        // This allows us to detect the case of there
        // being more than one byte in the pipe.

        uint8_t buffer[2];
        ssize_t count = read(m_pipeFD[0], buffer, 2);

        // Should have read exactly one byte,
        // leaving the pipe empty.
        if (count != 1)
        {
            std::ostringstream message;
            message << "FireableEvent::Reset(): read() unexpectedly returned "
                    <<  count;
            throwstd:: runtime_error{message.str()};
        }
    }

    // Clear any old associated data.
    SetAssociated(NULL);
    SetInt(-1);
}

