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

std::int_least64_t SurgeUtil::currentTimeMilliseconds()
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
