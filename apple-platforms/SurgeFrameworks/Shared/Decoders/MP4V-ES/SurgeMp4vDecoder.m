// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#import "SurgeMp4vDecoder.h"
#import "SurgeLogging.h"
#import "BitstreamReader.h"
#import "BitstreamWriter.h"

@interface SurgeMp4vDecoder ()
@property (nonatomic, assign) CMFormatDescriptionRef formatDescription;
@end


@implementation SurgeMp4vDecoder

- (void)decodeFrameBuffer:(const unsigned char*)frameBuffer
                   ofSize:(size_t)size
        withFrameDuration:(int)frameDuration
      andPresentationTime:(unsigned int)presentationTimeInterval {
    
    if (!self.formatDescription) {
        NSData *vosHeader = [self extractVisualObjectSequenceHeaderFrom:frameBuffer ofSize:size];
        CGSize dimensions = [self extractDimensionsFromVisualObjectSequenceHeader:[vosHeader bytes] ofSize:[vosHeader length]];
        if (CGSizeEqualToSize(dimensions, CGSizeZero)) {
            SurgeLogError(@"Failed to extract video dimensions from MPEG4 header, frame will be ignored");
            return;
        }

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
    timingInfo.duration = CMTimeMakeWithSeconds(frameDuration, 1);
    timingInfo.decodeTimeStamp = kCMTimeInvalid;
    timingInfo.presentationTimeStamp = CMTimeMakeWithSeconds(presentationTimeInterval, 1);
    
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

/**
 * See the VisualObjectSequence definition in the ISO/IEC 14496-2 (MPEG4 Visual) specification.
 */
- (CGSize)extractDimensionsFromVisualObjectSequenceHeader:(uint8_t *)vosHeader ofSize:(size_t)size {
    for (int i = 0; i < size - 4; i++) {
        if ([self isVideoObjectLayerStartCode:vosHeader + i]) {
            return [self extractDimensionsFromVideoObjectLayerHeader:vosHeader + i + 4 size:size - i - 4];
        }
    }
    return CGSizeZero;
}

- (BOOL)isVideoObjectLayerStartCode:(const uint8_t *)descriptor {
    const uint8_t startCodePrefix[] = { 0x00, 0x00, 0x01 };
    uint8_t startCode = *(descriptor + 3);
    if (memcmp(descriptor, startCodePrefix, 3) == 0 && (startCode >= 0x20 && startCode <= 0x2f)) {
        return YES;
    }
    return NO;
}

/**
 * See the VideoObjectLayer definition in the ISO/IEC 14496-2 (MPEG4 Visual) specification.
 */
- (CGSize)extractDimensionsFromVideoObjectLayerHeader:(uint8_t*)volHeader size:(size_t)size {
    BitstreamReader *videoObjectLayer = [[BitstreamReader alloc] initWithBuffer:volHeader ofSize:size];
    [videoObjectLayer skipNumberOfBits:9];
    uint8_t objectLayerIdentifier = [videoObjectLayer readBit];
    uint8_t objectLayerVerId = 0;
    if (objectLayerIdentifier) {
        objectLayerVerId = [videoObjectLayer readNumberOfBits:4];
        [videoObjectLayer skipNumberOfBits:3];
    }

    uint8_t aspectRatioInfo = [videoObjectLayer readNumberOfBits:4];
    if (aspectRatioInfo == 0b1111) {
        uint8_t width = [videoObjectLayer readByte];
        uint8_t height = [videoObjectLayer readByte];
        return CGSizeMake(width, height);
    }
    
    uint8_t controlParameters = [videoObjectLayer readBit];
    if (controlParameters) {
        [videoObjectLayer skipNumberOfBits:3];
        uint8_t vbvParameters = [videoObjectLayer readBit];
        if (vbvParameters) {
            [videoObjectLayer skipNumberOfBits:79];
        }
    }
    
    uint8_t videoObjectLayerShape = [videoObjectLayer readNumberOfBits:2];
    if (videoObjectLayerShape == 0b00 && objectLayerVerId != 0b0001) {
        [videoObjectLayer skipNumberOfBits:4];
    }
    
    [videoObjectLayer skipNumberOfBits:1];
    uint16_t vopTimeIncrementResolution = [videoObjectLayer readTwoBytes];
    [videoObjectLayer skipNumberOfBits:1];
    uint8_t fixedVopRate = [videoObjectLayer readBit];
    if (fixedVopRate) {
        [videoObjectLayer skipNumberOfBits:[self numberOfBitsRequiredtoStoreValue:vopTimeIncrementResolution]];
    }
    
    if (videoObjectLayerShape != 0b10) {
        if (videoObjectLayerShape == 0) {
            [videoObjectLayer skipNumberOfBits:1];
            uint32_t width = [videoObjectLayer readNumberOfBits:13];
            [videoObjectLayer skipNumberOfBits:1];
            uint32_t height = [videoObjectLayer readNumberOfBits:13];
            return CGSizeMake(width, height);
        }
    }
    
    return CGSizeZero;
}

- (uint32_t)numberOfBitsRequiredtoStoreValue:(uint32_t)value {
    return floor(log2(value)) + 1;
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
    uint32_t lengthOfDecoderConfigDescriptor = 5 + 13 + 5 + [vosHeader length];
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
    uint32_t lengthOfDecoderSpecificConfigDescriptor = 5 + [vosHeader length];
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
    [self appendLength:[vosHeader length] toDescriptor:descriptor];
    
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
