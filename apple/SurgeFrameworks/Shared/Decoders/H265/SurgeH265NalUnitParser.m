//
//  SurgeH265NalUnitParser.m
//  SurgeiOS
//
//  Created by Paul Shields on 05/11/2020.
//

#import "SurgeH265NalUnitParser.h"
#import "SurgeH265NalUnit.h"

@implementation SurgeH265NalUnitParser

+ (BOOL)arrayContainsNalUnit:(NSArray*)nalUnits ofType:(NSInteger)type {
    for (SurgeH265NalUnit *nalUnit in nalUnits) {
        if ([nalUnit type] == type) {
            return YES;
        }
    }
    return NO;
}

+ (NSArray*)filteredArrayOfNalUnits:(NSArray*)nalUnits ofType:(NSInteger)type {
    NSMutableArray *filteredNalUnits = [NSMutableArray new];
    for (SurgeH265NalUnit *nalUnit in nalUnits) {
        if ([nalUnit type] == type) {
            [filteredNalUnits addObject:nalUnit];
        }
    }
    return filteredNalUnits;
}

+ (NSArray*)parseNalUnitsFromFrameBuffer:(const unsigned char *)frameBuffer ofLength:(size_t)frameSize {
    NSMutableArray *nalUnits = [NSMutableArray new];

    size_t lastNalUnitStartingOffset = frameSize + 1;
    for (size_t i = 0; i < frameSize - 4; ++i) {
        if (frameBuffer[i] == 0x00 && frameBuffer[i+1] == 0x00 && frameBuffer[i+2] == 0x01) {
            uint32_t naluType = (frameBuffer[i+3] & 0x7E) >> 1;

            SurgeH265NalUnit *nalUnit = [[SurgeH265NalUnit alloc] initWithType:naluType atOffset:i withHeaderSize:3];
            if (i > 0 && frameBuffer[i-1] == 0x00) {
                // its actally a 4 byte code!
                [nalUnit setOffset:[nalUnit offset]-1];
                [nalUnit setHeaderSize:4];
            }

            if (lastNalUnitStartingOffset < frameSize) {
                SurgeH265NalUnit *lastNalUnit = [nalUnits lastObject];

                size_t length = [nalUnit offset] - [lastNalUnit offset];
                unsigned char *buffer = malloc(length);
                memcpy(buffer, frameBuffer + [lastNalUnit offset], length);
                [lastNalUnit setBuffer:buffer withLength:length];
                free(buffer);

                [nalUnits replaceObjectAtIndex:([nalUnits count] - 1) withObject:lastNalUnit];
            }

            [nalUnits addObject:nalUnit];
            lastNalUnitStartingOffset = [nalUnit offset];
        }
    }

    if ([nalUnits count]) {
        // fill out the last nalu buffer and length
        SurgeH265NalUnit *lastNalUnit = [nalUnits lastObject];

        size_t length = frameSize - [lastNalUnit offset];
        unsigned char *buffer = malloc(length);
        memcpy(buffer, frameBuffer + [lastNalUnit offset], length);
        [lastNalUnit setBuffer:buffer withLength:length];
        free(buffer);

        [nalUnits replaceObjectAtIndex:([nalUnits count] - 1) withObject:lastNalUnit];
    }
    return nalUnits;
}


@end
