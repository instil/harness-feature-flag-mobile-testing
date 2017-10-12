// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "SurgeMp4vDecoder.h"
#import "SurgeLogging.h"
#import "BitstreamReader.h"
#import "BitstreamWriter.h"

@interface SurgeMp4vDecoder ()
@property (nonatomic, assign) CMFormatDescriptionRef formatDescription;
@end


@implementation SurgeMp4vDecoder

- (void)decodeFrameBuffer:(const uint8_t *)frameBuffer
                   ofSize:(size_t)size
           withDimensions:(CGSize)dimensions
         presentationTime:(unsigned int)presentationTime
                 duration:(int)duration {

    if (!self.formatDescription) {
        if (CGSizeEqualToSize(dimensions, CGSizeZero)) {
            SurgeLogError(@"No dimensions available for MPEG4 stream, frame will be ignored");
            return;
        }

        NSData *vosHeader = [self extractVisualObjectSequenceHeaderFrom:frameBuffer ofSize:size];
        NSDictionary *atoms = @{@"esds": [self createElementaryStreamDescriptorAtom:vosHeader]};
        NSMutableDictionary *decoderConfig = [[NSMutableDictionary alloc] init];
        [decoderConfig setValue:atoms forKey:(__bridge NSString *)kCMFormatDescriptionExtension_SampleDescriptionExtensionAtoms];
        
        CMFormatDescriptionRef formatDescription;
        OSStatus status = CMVideoFormatDescriptionCreate(kCFAllocatorDefault,
                                                         kCMVideoCodecType_MPEG4Video,
                                                         dimensions.width,
                                                         dimensions.height,
                                                         (__bridge CFDictionaryRef)decoderConfig,
                                                         &formatDescription);
        
        if (status == noErr) {
            self.formatDescription = formatDescription;
        } else {
            SurgeLogError(@"Failed to create format description for MPEG4 video: %d", status);
            return;
        }
    }
    
    CMBlockBufferRef blockBuffer = NULL;
    OSStatus status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                         (void*)frameBuffer,
                                                         size,
                                                         kCFAllocatorMalloc,
                                                         NULL,
                                                         0,
                                                         size,
                                                         0,
                                                         &blockBuffer);
    
    CMSampleTimingInfo timingInfo;
    timingInfo.duration = CMTimeMakeWithSeconds(duration, 1);
    timingInfo.decodeTimeStamp = kCMTimeInvalid;
    timingInfo.presentationTimeStamp = CMTimeMakeWithSeconds(presentationTime, 1);
    
    CMSampleBufferRef sampleBuffer = NULL;
    status = CMSampleBufferCreate(kCFAllocatorDefault,
                                  blockBuffer,
                                  true,
                                  NULL,
                                  NULL,
                                  self.formatDescription,
                                  1,
                                  1,
                                  &timingInfo,
                                  1,
                                  &size,
                                  &sampleBuffer);
    
    [self enqueueSampleBuffer:sampleBuffer];
}

/**
 * Extract the VisualObjectSequence header from the bitstream.
 */
- (NSData*)extractVisualObjectSequenceHeaderFrom:(const uint8_t *)frameBuffer ofSize:(size_t)size {
    const unsigned char videoObjectPlaneStartCode[] = { 0x00, 0x00, 0x01, 0xb6 };
    for (int i = 0; i < size - 4; i++) {
        if (memcmp(frameBuffer + i, videoObjectPlaneStartCode, 4) == 0) {
            return [NSData dataWithBytes:frameBuffer length:i];
        }
    }
    return nil;
}

/*
 * See https://developer.apple.com/library/mac/documentation/QuickTime/QTFF/QTFFChap3/qtff3.html#//apple_ref/doc/uid/TP40000939-CH205-124774
 * definition of the elementary stream descriptor atom.
 */
- (NSData*)createElementaryStreamDescriptorAtom:(NSData*)vosHeader {
    BitstreamWriter *descriptor = [[BitstreamWriter alloc] init];
    [self appendAtomHeader:descriptor];
    [self appendElementaryStreamDescriptor:descriptor vosHeader:vosHeader];
    return [descriptor data];
}

- (void)appendAtomHeader:(BitstreamWriter*)descriptor {
    // Version
    [descriptor writeByte:0];
    
    // Flags
    [descriptor writeThreeBytes:0];
}

/*
 * Elementary stream descriptor as defined in the ISO/IEC 14496-1 (MPEG4 Systems) specification.
 */
- (void)appendElementaryStreamDescriptor:(BitstreamWriter*)descriptor vosHeader:(NSData*)vosHeader {
    // Descriptor tag
    [descriptor writeByte:0x03];
    
    // Length
    uint8_t remainingLengthOfThisDescriptor = 3;
    uint32_t lengthOfDecoderConfigDescriptor = (uint32_t)(5 + 13 + 5 + [vosHeader length]);
    uint8_t lengthOfSyncLayerDescriptor = 3;
    uint32_t length = remainingLengthOfThisDescriptor + lengthOfDecoderConfigDescriptor + lengthOfSyncLayerDescriptor;
    [self appendLength:length toDescriptor:descriptor];
    
    // Elementary stream identifier
    [descriptor writeTwoBytes:0];
    
    // Stream dependence flag
    [descriptor writeBit:0];
    
    // URL flag
    [descriptor writeBit:0];
    
    // OCR stream flag
    [descriptor writeBit:0];
    
    // Stream priority
    [descriptor writeBits:0 numberOfBits:5];
    
    [self appendDecoderConfigDescriptor:descriptor vosHeader:vosHeader];
    [self appendSyncLayerDescriptor:descriptor];
}

- (void)appendDecoderConfigDescriptor:(BitstreamWriter*)descriptor vosHeader:(NSData*)vosHeader {
    // Descriptor tag
    [descriptor writeByte:0x04];
    
    // Length
    uint8_t remainingLengthOfThisDescriptor = 13;
    uint32_t lengthOfDecoderSpecificConfigDescriptor = (uint32_t)(5 + [vosHeader length]);
    uint32_t length = remainingLengthOfThisDescriptor + lengthOfDecoderSpecificConfigDescriptor;
    [self appendLength:length toDescriptor:descriptor];
    
    // Object type indication
    [descriptor writeByte:0x20];
    
    // Stream type
    [descriptor writeBits:0x04 numberOfBits:6];
    
    // Upstream
    [descriptor writeBit:0];
    
    // Reserved bit
    [descriptor writeBit:0];
    
    // Buffer size
    [descriptor writeThreeBytes:0];
    
    // Max bitrate
    [descriptor writeFourBytes:0];
    
    // Average bitrate
    [descriptor writeFourBytes:0];
    
    [self appendDecoderSpecificConfig:descriptor vosHeader:vosHeader];
}

- (void)appendDecoderSpecificConfig:(BitstreamWriter*)descriptor vosHeader:(NSData*)vosHeader {
    // Descriptor tag
    [descriptor writeByte:0x05];
    
    // Length
    [self appendLength:(uint32_t)[vosHeader length] toDescriptor:descriptor];
    
    // VOS header
    [descriptor writeBytes:vosHeader];
}

- (void)appendSyncLayerDescriptor:(BitstreamWriter*)descriptor {
    // Descriptor tag
    [descriptor writeByte:0x06];
    
    // Length
    [descriptor writeByte:0x01];
    
    // Sync layer packet header
    [descriptor writeByte:0x02];
}

- (void)appendLength:(uint32_t)length toDescriptor:(BitstreamWriter*)descriptor {
    for (int i = 3; i > 0; i--) {
        [descriptor writeByte:(length >> (7 * i)) | 0x80];
    }
    [descriptor writeByte:length & 0x7f];
}


@end
