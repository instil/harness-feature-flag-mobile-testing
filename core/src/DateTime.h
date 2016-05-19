// -*-c++-*-
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
