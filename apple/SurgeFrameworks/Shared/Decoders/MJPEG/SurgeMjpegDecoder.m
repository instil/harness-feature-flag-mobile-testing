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

#import "SurgeMjpegDecoder.h"
#import "SurgeLogging.h"

@interface SurgeDecoder()
@property (nonatomic, assign) int framePerSecondCounter;
@end

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

    CFRelease(cfdata);

    CGImageRelease(image);
    CGDataProviderRelease(imageDataProvider);
    
    self.framePerSecondCounter++;
}

@end
