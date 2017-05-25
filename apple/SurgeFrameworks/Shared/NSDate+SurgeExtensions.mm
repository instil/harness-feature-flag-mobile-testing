// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "NSDate+SurgeExtensions.h"

@implementation NSDate (SurgeExtensions)

- (SurgeUtil::DateTime) toSurgeDateTime {
    SurgeUtil::DateTime dateTime = SurgeUtil::DateTime();
    
    NSCalendar *calendar = [NSCalendar currentCalendar];
    [calendar setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
    
    NSDateComponents *components = [calendar components:(NSCalendarUnitTimeZone |
                                                         NSCalendarUnitDay |
                                                         NSCalendarUnitMonth |
                                                         NSCalendarUnitYear |
                                                         NSCalendarUnitHour |
                                                         NSCalendarUnitMinute |
                                                         NSCalendarUnitSecond |
                                                         NSCalendarUnitNanosecond)
                                               fromDate:self];

    dateTime.Year = [components year];
    dateTime.Month = [components month];
    dateTime.Day = [components day];
    dateTime.Hour = [components hour];
    dateTime.Minute = [components minute];
    dateTime.Second = [components second];
    dateTime.Nanosecond = [components nanosecond];
    
    return dateTime;
}

@end
