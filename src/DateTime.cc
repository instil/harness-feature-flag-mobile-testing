// Standard C++ Headers
#include <chrono>
#include <ctime>

// SurgeUtil Headers
#include "DateTime.h"
  using SurgeUtil::DateTime;
  using std::int_least64_t;            // From included <cstdint>

DateTime SurgeUtil::CurrentDateTime()
{
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    
    //tm utc_tm = *gmtime(&tt);
    tm local_tm = *localtime(&tt);

    DateTime result;
    result.Year       = local_tm.tm_year + 1900;
    result.Month      = local_tm.tm_mon + 1;
    result.Day        = local_tm.tm_mday;
    result.Hour       = local_tm.tm_hour;
    result.Minute     = local_tm.tm_min;
    result.Second     = local_tm.tm_sec;
    result.Nanosecond = 0; //static_cast<int_least32_t>(time.tv_nsec);

    return result;
}

std::int_least64_t SurgeUtil::CurrentTimeMilliseconds()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
