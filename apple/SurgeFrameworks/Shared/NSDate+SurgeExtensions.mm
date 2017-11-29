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

    dateTime.Year = (int)[components year];
    dateTime.Month = (int)[components month];
    dateTime.Day = (int)[components day];
    dateTime.Hour = (int)[components hour];
    dateTime.Minute = (int)[components minute];
    dateTime.Second = (int)[components second];
    dateTime.Nanosecond = (int)[components nanosecond];
    
    return dateTime;
}

@end
