//
//  NaluParser.m
//  H264DecoderWrapper
//
//  Created by Philip Herron on 24/08/2015.
//  Copyright (c) 2015 Instil Software. All rights reserved.
//

#import "NaluParser.h"
#import "NaluSegment.h"
#import "NaluSegmentTypes.h"

@implementation NaluParser

+ (BOOL)segmentList:(NSArray*)nalus containsNalu:(NSInteger)type {
    for (NaluSegment *segment in nalus) {
        if ([segment type] == type) {
            return YES;
        }
    }
    return NO;
}

+ (NSArray*)findNalusOfType:(NSInteger)type inArray:(NSArray*)nalus {
    NSMutableArray *segments = [NSMutableArray new];
    for (NaluSegment *segment in nalus) {
        if ([segment type] == type) {
            [segments addObject:segment];
        }
    }
    return segments;
}

/**
 The frame buffer is comprised of nalu's - sequences of <magic byte header>, <type>, <data section>.
 
 XXX Y ZZZZZZZZZZZZZ -> XXX Y ZZZZZZZZZZZZZ -> XXX Y ZZZZZZZZZZZZZ
 
 XXX  - Magic Byte sequence 0x00, 0x00, 0x00, 0x01 (This can be a 3 or 4 byte sequence).
 Y    - NALU type (frameBuffer[...] & 0x1F) will fix the endianess of the type to a useable int.
 Z... - This is the sequence of bytes that make up that NALU.
 
 No data size is specified which means, we have to parse along to the next magic byte sequence or to the end of the buffer
 to know the size of the full nalu segment.
 **/
+ (NSArray*)parseOutNalusFromFrameBuffer:(const unsigned char *)frameBuffer ofLength:(size_t)frameSize {
    NSMutableArray *nalus = [NSMutableArray new];
    
    size_t lastNaluStartingOffset = frameSize + 1;
    for (size_t i = 0; i < frameSize - 4; ++i) {
        if (frameBuffer[i] == 0x00 && frameBuffer[i+1] == 0x00 && frameBuffer[i+2] == 0x01) {
            uint32_t naluType = (frameBuffer[i+3] & 0x1F);
            
            NaluSegment *segment = [[NaluSegment alloc] initWithType:naluType atOffset:i withHeaderSize:3];
            if (i > 0 && frameBuffer[i-1] == 0x00) {
                // its actally a 4 byte code!
                [segment setOffset:[segment offset]-1];
                [segment setHeaderSize:4];
            }
            
            if (lastNaluStartingOffset < frameSize) {
                NaluSegment *lastSegment = [nalus lastObject];
                
                size_t length = [segment offset] - [lastSegment offset];
                unsigned char *buffer = malloc(length);
                memcpy(buffer, frameBuffer + [lastSegment offset], length);
                [lastSegment setBuffer:buffer withLength:length];
                free(buffer);
                
                [nalus replaceObjectAtIndex:([nalus count] - 1) withObject:lastSegment];
            }
            
            [nalus addObject:segment];
            lastNaluStartingOffset = [segment offset];
        }
    }
    
    if ([nalus count]) {
        // fill out the last nalu buffer and length
        NaluSegment *lastSegment = [nalus lastObject];

        size_t length = frameSize - [lastSegment offset];
        unsigned char *buffer = malloc(length);
        memcpy(buffer, frameBuffer + [lastSegment offset], length);
        [lastSegment setBuffer:buffer withLength:length];
        free(buffer);
        
        [nalus replaceObjectAtIndex:([nalus count] - 1) withObject:lastSegment];
    }
    return nalus;
}

@end
