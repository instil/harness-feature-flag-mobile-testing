// Standard C++ Headers
#include <sstream>
  using std::ostringstream;
#include <stdexcept>
  using std::runtime_error;
#include <system_error>
  using std::system_error;

#include "Mutex.h"
  using SurgeUtil::Mutex;

void Mutex::Lock()
{
    try
    {
        m_mutex.lock();
    }
    catch (system_error e)
    {
        ostringstream message;
        message << "Mutex::Lock(): failed to lock: "
                << "error code = " << e.code();
        throw runtime_error{message.str()};
    }
}

void Mutex::Unlock()
{
    try
    {
        m_mutex.unlock();
    }
    catch (system_error e)
    {
        ostringstream message;
        message << "Mutex::Unlock(): failed to unlock: "
                << "error code = " << e.code();
        throw runtime_error{message.str()};
    }
}

