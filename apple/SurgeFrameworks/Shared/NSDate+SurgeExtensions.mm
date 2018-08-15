// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "NSDate+SurgeExtensions.h"

@implementation NSDate (SurgeExtensions)

- (SurgeUtil::DateTime *) toSurgeDateTime {
    
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

    return new SurgeUtil::DateTime((int)[components year],
                                   (int)[components month],
                                   (int)[components day],
                                   (int)[components hour],
                                   (int)[components minute],
                                   (int)[components second],
                                   (int)[components nanosecond]);
}

@end
