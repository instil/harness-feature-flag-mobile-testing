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

#import "SurgeDecoder.h"
#import "SurgeLogging.h"

@interface SurgeDecoder()
@property (nonatomic, strong) dispatch_queue_t decoderQueue;
@property (nonatomic, assign) CMVideoFormatDescriptionRef formatDescription;
@property (nonatomic, assign) VTDecompressionSessionRef decompressionSession;
@property (nonatomic, assign) int framePerSecondCounter;
@end


@implementation SurgeDecoder

- (id)initWithDelegate:(id<SurgeDecoderDelegate>)delegate {
    if (self = [super init]) {
        self.delegate = delegate;
        self.decoderQueue = dispatch_queue_create("co.instil.decoder", DISPATCH_QUEUE_SERIAL);
        [self updateFramesPerSecond];
    }
    return self;
}

- (void)dealloc {
    dispatch_sync(self.decoderQueue, ^{});
    [self freeFormatDescription];
    [self freeVideoDecompressionSession];
}

- (void)freeFormatDescription {
    if (_formatDescription != NULL) {
        CFRelease(_formatDescription);
        _formatDescription = NULL;
    }
}

- (void)freeVideoDecompressionSession {
    if (_decompressionSession) {
        VTDecompressionSessionInvalidate(_decompressionSession);
        CFRelease(_decompressionSession);
        _decompressionSession = NULL;
    }
}

#pragma mark - Abstract methods

- (void)decodeFrameBuffer:(const unsigned char*)frameBuffer
                   ofSize:(size_t)size
        withFrameDuration:(int)frameDuration
      andPresentationTime:(unsigned int)presentationTimeInterval {
    
    @throw [NSException exceptionWithName:@"Not implemented" reason:@"Abstract method must implemented in sub-class" userInfo:nil];
}

#pragma mark - Decoding

- (void)enqueueSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    dispatch_async(self.decoderQueue, ^{
        CMFormatDescriptionRef formatDescription = CMSampleBufferGetFormatDescription(sampleBuffer);
        [self createDecompressionSessionIfRequired:formatDescription];
        if (self.formatDescription != NULL && CMFormatDescriptionEqual(self.formatDescription, formatDescription)) {
            [self decodeSampleBuffer:sampleBuffer];
        }
        CFRelease(sampleBuffer);
    });
}

- (void)createDecompressionSessionIfRequired:(CMFormatDescriptionRef)formatDescription {
    if (self.decompressionSession == NULL) {
        [self freeFormatDescription];
        self.formatDescription = formatDescription;
        [self createDecompressionSession];
    } else if (!VTDecompressionSessionCanAcceptFormatDescription(self.decompressionSession, formatDescription)) {
        [self freeFormatDescription];
        self.formatDescription = formatDescription;
        [self createDecompressionSession];
    }
}

- (void)createDecompressionSession {
    [self freeVideoDecompressionSession];
    
    VTDecompressionOutputCallbackRecord callBackRecord;
    callBackRecord.decompressionOutputCallback = decompressionSessionDecodeFrameCallback;
    callBackRecord.decompressionOutputRefCon = (__bridge void *)self;
    
#if TARGET_OS_IPHONE
    NSDictionary *destinationImageBufferAttributes = @{ (id)kCVPixelBufferOpenGLESCompatibilityKey: @YES,
                                                        (id)kCVPixelBufferCGImageCompatibilityKey: @YES };
#else
    NSDictionary *destinationImageBufferAttributes = @{ (id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA),
                                                        (id)kCVPixelBufferCGImageCompatibilityKey: @YES };
#endif
    
    OSStatus status =  VTDecompressionSessionCreate(NULL,
                                                    self.formatDescription,
                                                    NULL,
                                                    (__bridge CFDictionaryRef)(destinationImageBufferAttributes),
                                                    &callBackRecord,
                                                    &_decompressionSession);
    
    SurgeLogDebug(@"Video Decompression Session Create: \t %@", (status == noErr) ? @"successful!" : @"failed...");
    if(status != noErr) {
        SurgeLogError(@"\t\t VTD ERROR type: %d", (int)status);
    }
}

- (OSStatus)decodeSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    VTDecodeFrameFlags decodeFlags = (kVTDecodeFrame_EnableAsynchronousDecompression);
    return VTDecompressionSessionDecodeFrame(self.decompressionSession,
                                             sampleBuffer,
                                             decodeFlags,
                                             NULL,
                                             0);
}

#pragma mark - Decoded frame rendering

void decompressionSessionDecodeFrameCallback(void *decompressionOutputRefCon,
                                             void *sourceFrameRefCon,
                                             OSStatus status,
                                             VTDecodeInfoFlags infoFlags,
                                             CVImageBufferRef imageBuffer,
                                             CMTime presentationTimeStamp,
                                             CMTime presentationDuration) {
    
    SurgeDecoder *decoder = (__bridge SurgeDecoder *)decompressionOutputRefCon;
    if (status == noErr) {
        decoder.framePerSecondCounter++;
        [decoder renderFrame:imageBuffer withPresentationTime:presentationTimeStamp];
    } else {
        SurgeLogError(@"Decode failed: %i", status);
    }
}

- (void)renderFrame:(CVImageBufferRef)imageBuffer withPresentationTime:(CMTime)presentationTime {
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    void *lumaBuffer = CVPixelBufferGetBaseAddress(imageBuffer);
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    size_t bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(lumaBuffer,
                                                 width,
                                                 height,
                                                 bitsPerComponent,
                                                 bytesPerRow,
                                                 colorSpace,
                                                 kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
    CGImageRef dstImage = CGBitmapContextCreateImage(context);
    [self.delegate decoderFrameAvailable:dstImage withTimeStamp:presentationTime.value];
    CGImageRelease(dstImage);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
}

#pragma mark - Decoder statistics

- (void)updateFramesPerSecond {
   dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), self.decoderQueue, ^{
        self.framesPerSecond = self.framePerSecondCounter;
        self.framePerSecondCounter = 0;
        [self updateFramesPerSecond];
    });
}

@end