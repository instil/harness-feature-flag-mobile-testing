//
//  SurgeH265NalUnitParser.h
//  SurgeiOS
//
//  Created by Paul Shields on 05/11/2020.
//

#import <Foundation/Foundation.h>

@interface SurgeH265NalUnitParser : NSObject

+ (BOOL)arrayContainsNalUnit:(NSArray*)nalUnits ofType:(NSInteger)type;

+ (NSArray*)filteredArrayOfNalUnits:(NSArray*)nalUnits ofType:(NSInteger)type;

+ (NSArray*)parseNalUnitsFromFrameBuffer:(const unsigned char *)frameBuffer ofLength:(size_t)frameSize;

@end
