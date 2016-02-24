// Standard C++ Headers
#include <thread>
  using std::this_thread::sleep_for;
#include <chrono>
  using std::chrono::milliseconds;

// TycoUtility Headers
#include "Sleep.h"

void SurgeUtil::Sleep(const long a_milliseconds)
{
    try
    {
        sleep_for(milliseconds(a_milliseconds));
    }
    catch (...)
    {
        // Swallow this.
        // Standard C++ implementation does not throw exceptions.
    }
}

