// -*-c++-*-
#ifndef __DATETIME_H__
#define __DATETIME_H__

namespace SurgeUtil {
    /**
    * Holds date and time to nanosecond.
    */
    struct DateTime
    {
        int Year;                       // Year AD, eg. Battle Waterloo 1815.
        int Month;                      // 1-12, 1 = January
        int Day;                        // 1-31  (day of month)
        int Hour;                       // 0-23
        int Minute;                     // 0-59
        int Second;                     // 0-59
        std::int_least32_t Nanosecond;  // 0-999999999
    };

   /**
    * Current UTC date and time.
    */
    DateTime CurrentDateTime();

   /**
    * Current time in milliseconds since the clock epoch.
    *
    * @note   This is measured with a "steady" clock, so it
    *         will not be affected by system time adjustments.
    */
    std::int_least64_t CurrentTimeMilliseconds();
}

#endif //__DATETIME_H__
