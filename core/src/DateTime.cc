// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "DateTime.h"

SurgeUtil::DateTime SurgeUtil::currentDateTime()
{
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    
    // tm utc_tm = *gmtime(&tt);
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

long long int SurgeUtil::currentTimeMilliseconds()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

std::string SurgeUtil::convertToISO(const SurgeUtil::DateTime& value)
{
    char result[100];
    snprintf(result, sizeof(result), "%04d%02d%02dT%02d%02d%02d.%09dZ",
             value.Year,
             value.Month,
             value.Day,
             value.Hour,
             value.Minute,
             value.Second,
             value.Nanosecond);
    return result;
}
