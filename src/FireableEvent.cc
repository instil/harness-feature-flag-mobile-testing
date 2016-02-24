// Standard C++ Headers
#include <cerrno>
#include <cstdint>
  using std::uint8_t;
#include <sstream>
  using std::ostringstream;
#include <stdexcept>
  using std::runtime_error;

// System Headers
#include <sys/select.h>
#include <unistd.h>

// SurgeUtil Headers
#include "FireableEvent.h"
  using SurgeUtil::FireableEvent;
#include "Mutex_locker.h"
  using SurgeUtil::Mutex_locker;

FireableEvent::FireableEvent()
{
    if (pipe(m_pipeFD) == -1)
    {
        ostringstream message;
        message << "FireableEvent::FireableEvent(): failed to create pipe: "
                << "errno = " << errno;
        throw runtime_error{message.str()};
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

    Mutex_locker lock{m_pipeMutex};

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

    Mutex_locker lock{m_pipeMutex};

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
            ostringstream message;
            message << "FireableEvent::Reset(): read() unexpectedly returned "
                    <<  count;
            throw runtime_error{message.str()};
        }
    }

    // Clear any old associated data.
    SetAssociated(NULL);
    SetInt(-1);
}

