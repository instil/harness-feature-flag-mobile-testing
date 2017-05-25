// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.


#include <cerrno>
#include <cstdint>
#include <sstream>

#include <unistd.h>

#include "FireableEvent.h"
#include "MutexLocker.h"


SurgeUtil::FireableEvent::FireableEvent(): m_associatedData(nullptr),
                                           m_associatedInt(-1)      
{
    if (pipe(m_pipeFD) == -1)
    {
        std::ostringstream message;
        message << "FireableEvent::FireableEvent(): failed to create pipe: "
                << "error (" << errno << ") " << strerror(errno);
        throw std::runtime_error{message.str()};
    }
}

SurgeUtil::FireableEvent::~FireableEvent()
{
    close(m_pipeFD[0]);
    close(m_pipeFD[1]);
}

void SurgeUtil::FireableEvent::Fire()
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

void SurgeUtil::FireableEvent::Reset()
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
            throw std:: runtime_error{message.str()};
        }
    }

    // Clear any old associated data.
    SetAssociated(NULL);
    SetInt(-1);
}

