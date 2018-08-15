// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

#import "DateTime.h"

@interface NSDate (SurgeExtensions)

/**
 * Convert an NSDate object into a Surge compatible DateTime object
 */
- (SurgeUtil::DateTime *) toSurgeDateTime;

@end
