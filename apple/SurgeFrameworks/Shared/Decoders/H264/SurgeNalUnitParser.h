// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

#import "SurgeNalUnit.h"

@interface SurgeNalUnitParser : NSObject

+ (BOOL)arrayContainsNalUnit:(NSArray*)nalUnits ofType:(NSInteger)type;

+ (NSArray*)filteredArrayOfNalUnits:(NSArray*)nalUnits ofType:(NSInteger)type;

+ (NSArray*)parseNalUnitsFromFrameBuffer:(const unsigned char *)frameBuffer ofLength:(size_t)frameSize;

@end
