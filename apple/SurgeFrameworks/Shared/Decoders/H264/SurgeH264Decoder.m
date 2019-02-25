// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "SurgeH264Decoder.h"
#import "SurgeNalUnitParser.h"
#import "SurgeLogging.h"

@interface SurgeH264Decoder ()
@end

@implementation SurgeH264Decoder

#pragma mark - Decoder

- (void)decodeFrameBuffer:(const uint8_t *)frameBuffer
                   ofSize:(size_t)size
           withDimensions:(CGSize)dimensions
         presentationTime:(unsigned int)presentationTime
                 duration:(int)duration {

    NSArray *nalUnits = [SurgeNalUnitParser parseNalUnitsFromFrameBuffer:frameBuffer ofLength:size];
//    for (SurgeNalUnit *nalUnit in nalUnits) {
//        SurgeLogDebug(@"NALU Type : %lu", (unsigned long)nalUnit.type);
//    }
    
    [self setFormatDescriptionFromParameterSetsIfAvailable:nalUnits];
    
    size_t blockLength = 0;
    CMBlockBufferRef blockBuffer = NULL;
    BOOL foundIdrFrame = [SurgeNalUnitParser arrayContainsNalUnit:nalUnits ofType:SurgeNalUnitTypeIDR];
    BOOL foundNonIdrFrame = [SurgeNalUnitParser arrayContainsNalUnit:nalUnits ofType:SurgeNalUnitTypeNonIDR];
    if (foundIdrFrame || foundNonIdrFrame) {
        SurgeNalUnitType nalUnitType = (foundIdrFrame) ? SurgeNalUnitTypeIDR : SurgeNalUnitTypeNonIDR;
        
        NSArray *idrs = [SurgeNalUnitParser filteredArrayOfNalUnits:nalUnits ofType:nalUnitType];
        SurgeNalUnit *nalUnit = [idrs lastObject];
        blockLength = nalUnit.length;

        [self createBlockBuffer:&blockBuffer forNalUnit:nalUnit];
    }
    
    if (blockBuffer != NULL) {
        CMSampleBufferRef sampleBuffer = NULL;
        [self createSampleBuffer:&sampleBuffer
                  withBlockBuffer:&blockBuffer
                       ofLength:CMBlockBufferGetDataLength(blockBuffer)
             andNumberOfSamples:1
              withFrameDuration:duration
            andPresentationTime:presentationTime];

        [self enqueueSampleBuffer:sampleBuffer];

        CFRelease(blockBuffer);
    }
    
    for (SurgeNalUnit *nalUnit in nalUnits) {
        [nalUnit cleanup];
    }
}

- (void)setFormatDescriptionFromParameterSetsIfAvailable:(NSArray*)nalUnits {
    if ([SurgeNalUnitParser arrayContainsNalUnit:nalUnits ofType:SurgeNalUnitTypeSPS] && [SurgeNalUnitParser arrayContainsNalUnit:nalUnits ofType:SurgeNalUnitTypePPS]) {
        CMFormatDescriptionRef formatDescription;
        OSStatus status = [self createVideoFormatDescription:&formatDescription fromNalUnits:nalUnits];
        if (status == noErr) {
            self.formatDescription = formatDescription;
        }
    }
}

- (OSStatus)createVideoFormatDescription:(CMFormatDescriptionRef*)formatDescription fromNalUnits:(NSArray*)nalUnits {
    NSArray *pictureParamSet = [SurgeNalUnitParser filteredArrayOfNalUnits:nalUnits ofType:SurgeNalUnitTypePPS];
    NSArray *sequenceParamSet = [SurgeNalUnitParser filteredArrayOfNalUnits:nalUnits ofType:SurgeNalUnitTypeSPS];
    
    size_t totalNumberOfParameters = 2;
    uint8_t **parameterSetPointers = calloc(totalNumberOfParameters, sizeof(uint8_t*));
    size_t *parameterSetSizes = calloc(totalNumberOfParameters, sizeof(size_t));
    
    SurgeNalUnit *ppsNalUnit = pictureParamSet.lastObject;
    parameterSetSizes[0] = ppsNalUnit.length - ppsNalUnit.headerSize;
    parameterSetPointers[0] = [ppsNalUnit bufferWithoutHeader];
    
    SurgeNalUnit *spsNalUnit = sequenceParamSet.lastObject;
    parameterSetSizes[1] = spsNalUnit.length - spsNalUnit.headerSize;
    parameterSetPointers[1] = [spsNalUnit bufferWithoutHeader];

    OSStatus status = CMVideoFormatDescriptionCreateFromH264ParameterSets(kCFAllocatorDefault,
                                                                          totalNumberOfParameters,
                                                                          (const uint8_t *const*)parameterSetPointers,
                                                                          parameterSetSizes,
                                                                          4,
                                                                          formatDescription);
    free(parameterSetPointers);
    free(parameterSetSizes);
    
    return status;
}

- (OSStatus)createBlockBuffer:(CMBlockBufferRef*)blockBuffer forNalUnit:(SurgeNalUnit*)nalUnit {
    void *data = [self replaceNalUnitHeaderWithSize:nalUnit];
    OSStatus status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                         data,
                                                         nalUnit.length,
                                                         kCFAllocatorDefault,
                                                         NULL,
                                                         0,
                                                         nalUnit.length,
                                                         0,
                                                         blockBuffer);
    return status;
}

- (void*)replaceNalUnitHeaderWithSize:(SurgeNalUnit*)nalUnit {
    void *origionalData = [nalUnit annexBBuffer];
    void *data = malloc(nalUnit.length);
    memcpy(data, origionalData, nalUnit.length);
    uint32_t dataLength32 = htonl(nalUnit.length - 4);
    memcpy(data, &dataLength32, 4);
    return data;
}

- (OSStatus)createSampleBuffer:(CMSampleBufferRef*)sampleBuffer
               withBlockBuffer:(CMBlockBufferRef*)blockBuffer
                      ofLength:(size_t)blockLength
            andNumberOfSamples:(CMItemCount)numberOfSamples
             withFrameDuration:(int)frameDuration
           andPresentationTime:(unsigned int)presentationTime {
    
    CMSampleTimingInfo timingInfo;
    timingInfo.duration = CMTimeMakeWithSeconds(frameDuration, 1);
    timingInfo.decodeTimeStamp = kCMTimeInvalid;
    timingInfo.presentationTimeStamp = CMTimeMakeWithSeconds(presentationTime, 1);
    
    OSStatus result = CMSampleBufferCreate(kCFAllocatorDefault,
                                *blockBuffer,
                                true,
                                NULL,
                                NULL,
                                self.formatDescription,
                                numberOfSamples,
                                1,
                                &timingInfo,
                                numberOfSamples,
                                &blockLength,
                                sampleBuffer);

    return result;
}


@end
