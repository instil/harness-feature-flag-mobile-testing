// Copyright (c) 2020 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

#import "SurgeH265Decoder.h"
#import "SurgeH265NalUnitParser.h"
#import "SurgeH265NalUnit.h"
#import "SurgeLogging.h"

@implementation SurgeH265Decoder

- (void)decodeFrameBuffer:(const uint8_t *)frameBuffer
                   ofSize:(size_t)size
           withDimensions:(CGSize)dimensions
         presentationTime:(unsigned int)presentationTime
                 duration:(int)duration {

    NSArray *nalUnits = [SurgeH265NalUnitParser parseNalUnitsFromFrameBuffer:frameBuffer ofLength:size];

    [self setFormatDescriptionFromParameterSetsIfAvailable:nalUnits];

    CMBlockBufferRef blockBuffer = NULL;

    for (SurgeH265NalUnit *nalUnit in nalUnits) {
        if (nalUnit.type != SurgeH265NalUnitSPS
            && nalUnit.type != SurgeH265NalUnitPPS
            && nalUnit.type != SurgeH265NalUnitVPS
            && nalUnit.type != SurgeH265NalUnitSEI) {
            [self createBlockBuffer:&blockBuffer forNalUnit:nalUnit];
        }
    }

    if (blockBuffer != NULL) {
        CMSampleBufferRef sampleBuffer = NULL;
        [self createSampleBuffer:&sampleBuffer
                  withBlockBuffer:&blockBuffer
                       ofLength:CMBlockBufferGetDataLength(blockBuffer)
             andNumberOfSamples:1
              withFrameDuration:duration
            andPresentationTime:presentationTime andSomeSizes:NULL];

        [self enqueueSampleBuffer:sampleBuffer];

        CFRelease(blockBuffer);
    }

    for (SurgeH265NalUnit *nalUnit in nalUnits) {
        [nalUnit cleanup];
    }

    [self.diagnostics trackNewFrameOfSize:size * 8];
}


- (void)setFormatDescriptionFromParameterSetsIfAvailable:(NSArray*)nalUnits {
    if ([SurgeH265NalUnitParser arrayContainsNalUnit:nalUnits ofType:SurgeH265NalUnitVPS]
        && [SurgeH265NalUnitParser arrayContainsNalUnit:nalUnits ofType:SurgeH265NalUnitSPS]
        && [SurgeH265NalUnitParser arrayContainsNalUnit:nalUnits ofType:SurgeH265NalUnitPPS]) {
        CMFormatDescriptionRef formatDescription;
        OSStatus status = [self createVideoFormatDescription:&formatDescription fromNalUnits:nalUnits];
        if (status == noErr) {
            self.formatDescription = formatDescription;
        }
    }
}


- (OSStatus)createVideoFormatDescription:(CMFormatDescriptionRef*)formatDescription fromNalUnits:(NSArray*)nalUnits {
    NSArray *pictureParamSet = [SurgeH265NalUnitParser filteredArrayOfNalUnits:nalUnits ofType:SurgeH265NalUnitPPS];
    NSArray *sequenceParamSet = [SurgeH265NalUnitParser filteredArrayOfNalUnits:nalUnits ofType:SurgeH265NalUnitSPS];
    NSArray *videoParamSet = [SurgeH265NalUnitParser filteredArrayOfNalUnits:nalUnits ofType:SurgeH265NalUnitVPS];

    size_t totalNumberOfParameters = 3;
    uint8_t **parameterSetPointers = calloc(totalNumberOfParameters, sizeof(uint8_t*));
    size_t *parameterSetSizes = calloc(totalNumberOfParameters, sizeof(size_t));

    uint index = 0;

    SurgeH265NalUnit *vpsNalUnit = videoParamSet.firstObject;
    parameterSetSizes[index] = vpsNalUnit.length - vpsNalUnit.headerSize;
    parameterSetPointers[index++] = [vpsNalUnit bufferWithoutHeader];

    SurgeH265NalUnit *spsNalUnit = sequenceParamSet.firstObject;
    parameterSetSizes[index] = spsNalUnit.length - spsNalUnit.headerSize;
    parameterSetPointers[index++] = [spsNalUnit bufferWithoutHeader];

    SurgeH265NalUnit *ppsNalUnit = pictureParamSet.firstObject;
    parameterSetSizes[index] = ppsNalUnit.length - ppsNalUnit.headerSize;
    parameterSetPointers[index++] = [ppsNalUnit bufferWithoutHeader];

    if (@available(iOS 11.0, *)) {
        OSStatus status = CMVideoFormatDescriptionCreateFromHEVCParameterSets(kCFAllocatorDefault,
                                                                              totalNumberOfParameters,
                                                                              (const uint8_t *const*)parameterSetPointers,
                                                                              parameterSetSizes,
                                                                              4,
                                                                              NULL,
                                                                              formatDescription);
        free(parameterSetPointers);
        free(parameterSetSizes);

        return status;
    } else {
        // Fallback on earlier versions
    }

    return NULL;
}

- (OSStatus)createBlockBuffer:(CMBlockBufferRef*)blockBuffer forNalUnit:(SurgeH265NalUnit*)nalUnit {
    void *data = [self replaceNalUnitHeaderWithSize:nalUnit];
    OSStatus status = AVErrorUnknown;
    if (CMBlockBufferIsEmpty(*blockBuffer)) {
        status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                             data,
                                                             nalUnit.length,
                                                             kCFAllocatorDefault,
                                                             NULL,
                                                             0,
                                                             nalUnit.length,
                                                             0,
                                                             blockBuffer);
    } else {
        status = CMBlockBufferAppendMemoryBlock(*blockBuffer,
                                                data,
                                                nalUnit.length,
                                                kCFAllocatorDefault,
                                                NULL,
                                                0,
                                                nalUnit.length,
                                                0);
    }

    return status;
}

- (void*)replaceNalUnitHeaderWithSize:(SurgeH265NalUnit*)nalUnit {
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
           andPresentationTime:(unsigned int)presentationTime andSomeSizes:(size_t *)sizes {

    CMSampleTimingInfo timingInfo;
    timingInfo.duration = CMTimeMakeWithSeconds(frameDuration, 1);
    timingInfo.decodeTimeStamp = kCMTimeInvalid;
    timingInfo.presentationTimeStamp = CMTimeMakeWithSeconds(presentationTime, 1);

    return CMSampleBufferCreate(kCFAllocatorDefault,
                                *blockBuffer,
                                true,
                                NULL,
                                NULL,
                                self.formatDescription,
                                numberOfSamples,
                                1,
                                &timingInfo,
                                1,
                                &blockLength,
                                sampleBuffer);
}

@end
