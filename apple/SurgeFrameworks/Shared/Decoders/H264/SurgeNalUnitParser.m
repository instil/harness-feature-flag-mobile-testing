// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "SurgeNalUnitParser.h"

@implementation SurgeNalUnitParser

+ (BOOL)arrayContainsNalUnit:(NSArray*)nalUnits ofType:(NSInteger)type {
    for (SurgeNalUnit *nalUnit in nalUnits) {
        if ([nalUnit type] == type) {
            return YES;
        }
    }
    return NO;
}

+ (NSArray*)filteredArrayOfNalUnits:(NSArray*)nalUnits ofType:(NSInteger)type {
    NSMutableArray *filteredNalUnits = [NSMutableArray new];
    for (SurgeNalUnit *nalUnit in nalUnits) {
        if ([nalUnit type] == type) {
            [filteredNalUnits addObject:nalUnit];
        }
    }
    return filteredNalUnits;
}

/**
 The frame buffer is comprised of nal units - sequences of <magic byte header>, <type>, <data section>.
 
 XXX Y ZZZZZZZZZZZZZ -> XXX Y ZZZZZZZZZZZZZ -> XXX Y ZZZZZZZZZZZZZ
 
 XXX  - Magic Byte sequence 0x00, 0x00, 0x00, 0x01 (This can be a 3 or 4 byte sequence).
 Y    - NALU type (frameBuffer[...] & 0x1F) will fix the endianess of the type to a useable int.
 Z... - This is the sequence of bytes that make up that NALU.
 
 No data size is specified which means, we have to parse along to the next magic byte sequence or to the end of the buffer
 to know the size of the full nalu segment.
 **/
+ (NSArray*)parseNalUnitsFromFrameBuffer:(const unsigned char *)frameBuffer ofLength:(size_t)frameSize {
    NSMutableArray *nalUnits = [NSMutableArray new];
    
    size_t lastNalUnitStartingOffset = frameSize + 1;
    for (size_t i = 0; i < frameSize - 4; ++i) {
        if (frameBuffer[i] == 0x00 && frameBuffer[i+1] == 0x00 && frameBuffer[i+2] == 0x01) {
            uint32_t naluType = (frameBuffer[i+3] & 0x1F);
            
            SurgeNalUnit *nalUnit = [[SurgeNalUnit alloc] initWithType:naluType atOffset:i withHeaderSize:3];
            if (i > 0 && frameBuffer[i-1] == 0x00) {
                // its actally a 4 byte code!
                [nalUnit setOffset:[nalUnit offset]-1];
                [nalUnit setHeaderSize:4];
            }
            
            if (lastNalUnitStartingOffset < frameSize) {
                SurgeNalUnit *lastNalUnit = [nalUnits lastObject];
                
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
        SurgeNalUnit *lastNalUnit = [nalUnits lastObject];

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
