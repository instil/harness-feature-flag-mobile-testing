// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "SurgeMjpegDecoder.h"
#import "SurgeLogging.h"

@implementation SurgeMjpegDecoder

- (void)decodeFrameBuffer:(const uint8_t *)frameBuffer
                   ofSize:(size_t)size
           withDimensions:(CGSize)dimensions
         presentationTime:(unsigned int)presentationTime
                 duration:(int)duration {

    CFDataRef cfdata = CFDataCreate(NULL, frameBuffer, size);
    CGDataProviderRef imageDataProvider = CGDataProviderCreateWithCFData(cfdata);
    CGImageRef image = CGImageCreateWithJPEGDataProvider(imageDataProvider, NULL, true, kCGRenderingIntentDefault);
    
    [self.delegate decoderFrameAvailable:image withTimeStamp:presentationTime];

    [self.diagnostics trackNewFrameOfSize:size * 8];
    [self.diagnostics trackNewFrameDimensionsWithWidth:CGImageGetWidth(image)
                                             andHeight:CGImageGetHeight(image)];

    CFRelease(cfdata);

    CGImageRelease(image);
    CGDataProviderRelease(imageDataProvider);
}

- (SurgeMediaFormat)mediaFormat {
    return SurgeMjpeg;
}

@end
