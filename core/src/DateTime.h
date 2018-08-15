// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __DATETIME_H__
#define __DATETIME_H__

#include <chrono>
#include <string>

namespace SurgeUtil {

    /**
     * Holds date and time to nanosecond.
     */
    class DateTime
    {
    public:
        DateTime(int year, int month, int day, int hour, int minute, int second, std::int_least32_t nanosecond) {
            this->Year = year;
            this->Month = month;
            this->Day = day;
            this->Hour = hour;
            this->Minute = minute;
            this->Second = second;
            this->Nanosecond = nanosecond;
        }

        std::string ConvertToISO() {
            char result[100];
            snprintf(result, sizeof(result), "%04d%02d%02dT%02d%02d%02d.%09dZ",
                     this->Year,
                     this->Month,
                     this->Day,
                     this->Hour,
                     this->Minute,
                     this->Second,
                     this->Nanosecond);
            return result;
        }

    public:
        static DateTime CurrentTime() {
            auto now = std::chrono::system_clock::now();
            time_t tt = std::chrono::system_clock::to_time_t(now);

            // tm utc_tm = *gmtime(&tt);
            tm local_tm = *localtime(&tt);

            return SurgeUtil::DateTime(local_tm.tm_year + 1900,
                                       local_tm.tm_mon + 1,
                                       local_tm.tm_mday,
                                       local_tm.tm_hour,
                                       local_tm.tm_min,
                                       local_tm.tm_sec,
                                       0); //static_cast<int_least32_t>(time.tv_nsec);
        }

        static DateTime *CurrentTimeAsPointer() {
            auto now = std::chrono::system_clock::now();
            time_t tt = std::chrono::system_clock::to_time_t(now);

            // tm utc_tm = *gmtime(&tt);
            tm local_tm = *localtime(&tt);

            return new SurgeUtil::DateTime(local_tm.tm_year + 1900,
                                           local_tm.tm_mon + 1,
                                           local_tm.tm_mday,
                                           local_tm.tm_hour,
                                           local_tm.tm_min,
                                           local_tm.tm_sec,
                                           0); //static_cast<int_least32_t>(time.tv_nsec);
        }

        static long long int CurrentTimeInMilliseconds() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now().time_since_epoch()).count();
        }

    public:
        int Year;                       // Year AD, eg. Battle Waterloo 1815.
        int Month;                      // 1-12, 1 = January
        int Day;                        // 1-31  (day of month)
        int Hour;                       // 0-23
        int Minute;                     // 0-59
        int Second;                     // 0-59
        std::int_least32_t Nanosecond;  // 0-999999999
    };
}


//
//namespace SurgeUtil {
//
//    /**
//    * Holds date and time to nanosecond.
//    */
//    struct DateTime
//    {
//        int Year;                       // Year AD, eg. Battle Waterloo 1815.
//        int Month;                      // 1-12, 1 = January
//        int Day;                        // 1-31  (day of month)
//        int Hour;                       // 0-23
//        int Minute;                     // 0-59
//        int Second;                     // 0-59
//        std::int_least32_t Nanosecond;  // 0-999999999
//    };
//
//   /**
//    * Current UTC date and time.
//    */
//    DateTime currentDateTime();
//
//   /**
//    * Current time in milliseconds since the clock epoch.
//    *
//    * @note   This is measured with a "steady" clock, so it
//    *         will not be affected by system time adjustments.
//    */
//   long long int currentTimeMilliseconds();
//
//    /**
//     * Convert a DateTime object into an ISO 8601 string
//     */
//    std::string convertToISO(const SurgeUtil::DateTime& value);
//
//    /**
//     * A null equivalence for the DateTime object.
//     */
//    SurgeUtil::DateTime emptyDateTime();
//
//    /**
//     * Returns true if the supplied datetime is empty.
//     */
//    bool dateTimeIsEmpty(const SurgeUtil::DateTime &dateTime);
//
//}

#endif //__DATETIME_H__
