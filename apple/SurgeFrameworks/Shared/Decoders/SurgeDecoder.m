// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "SurgeDecoder.h"
#import "SurgeLogging.h"

@interface SurgeDecoder()
@property (nonatomic, strong) dispatch_queue_t decoderQueue;
@property (nonatomic, strong) dispatch_queue_t framerateQueue;
@property (nonatomic, assign) VTDecompressionSessionRef decompressionSession;
@end


@implementation SurgeDecoder

- (id)initWithDelegate:(id<SurgeDecoderDelegate>)delegate {
    if (self = [super init]) {
        self.delegate = delegate;
        self.decoderQueue = dispatch_queue_create("co.instil.decoder", DISPATCH_QUEUE_SERIAL);
    }
    return self;
}

- (void)dealloc {
    [self deinit];
}

- (void)deinit {
    SurgeLogInfo("Dealloc called");
    [self freeFormatDescription];
    [self freeVideoDecompressionSession];
}

- (void)flushDecoderQueue {
    dispatch_sync(self.decoderQueue, ^{});
}

- (void)freeFormatDescription {
    if (_formatDescription) {
        CFRelease(_formatDescription);
        _formatDescription = NULL;
    }
}

- (void)freeVideoDecompressionSession {
    if (_decompressionSession) {
        SurgeLogInfo("Decompression Session freed");
        VTDecompressionSessionWaitForAsynchronousFrames(_decompressionSession);
        VTDecompressionSessionInvalidate(_decompressionSession);
        CFRelease(_decompressionSession);
        _decompressionSession = NULL;
    }
}

#pragma mark - Abstract methods

- (void)decodeFrameBuffer:(const uint8_t *)frameBuffer
                   ofSize:(size_t)size
           withDimensions:(CGSize)dimensions
         presentationTime:(unsigned int)presentationTime
                 duration:(int)duration {
    
    @throw [NSException exceptionWithName:@"Not implemented" reason:@"Abstract method must implemented in sub-class" userInfo:nil];
}

#pragma mark - Decoding

- (void)enqueueSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    dispatch_async(self.decoderQueue, ^{
        CMFormatDescriptionRef formatDescription = CMSampleBufferGetFormatDescription(sampleBuffer);
        if (self.formatDescription != NULL &&
            CMFormatDescriptionEqual(self.formatDescription, formatDescription)) {
            [self decodeSampleBuffer:sampleBuffer];
        }
        CFRelease(sampleBuffer);
    });
}

- (void)createDecompressionSessionIfRequired:(CMFormatDescriptionRef)formatDescription {
    if (CMFormatDescriptionEqual(self.formatDescription, formatDescription)) {
        [self createDecompressionSession:formatDescription];
    }
}

- (void)createDecompressionSession:(CMFormatDescriptionRef)formatDescription {
    [self freeVideoDecompressionSession];

    VTDecompressionOutputCallbackRecord callBackRecord;
    callBackRecord.decompressionOutputCallback = decompressionSessionDecodeFrameCallback;
    callBackRecord.decompressionOutputRefCon = (__bridge void *)self;
    
    NSMutableDictionary *destinationImageBufferAttributes = [[NSMutableDictionary alloc] init];
    NSMutableDictionary *decoderSpecification = [[NSMutableDictionary alloc] init];

    [destinationImageBufferAttributes setValue:@(kCVPixelFormatType_32BGRA) forKey:(__bridge NSString *)kCVPixelBufferPixelFormatTypeKey];
    [destinationImageBufferAttributes setValue:@YES forKey:(__bridge NSString *)kCVPixelBufferCGImageCompatibilityKey];

    OSStatus status =  VTDecompressionSessionCreate(NULL,
                                                    formatDescription,
                                                    (__bridge CFDictionaryRef)decoderSpecification,
                                                    (__bridge CFDictionaryRef)destinationImageBufferAttributes,
                                                    &callBackRecord,
                                                    &_decompressionSession);
    if(status != noErr) {
        SurgeLogError(@"Failed to create video decompression session with error code %d", (int)status);
    } else {
        SurgeLogDebug(@"Successfully created video decompression session");
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
        [decoder renderFrame:imageBuffer withPresentationTime:presentationTimeStamp];
    } else {
        SurgeLogError(@"Failed to decode frame with error code %i", status);
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
    CGImageRef image = CGBitmapContextCreateImage(context);
    [self.delegate decoderFrameAvailable:image withTimeStamp:presentationTime.value];
    [self.diagnostics trackNewFrameDimensionsWithWidth:width andHeight:height];
    CGImageRelease(image);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
}

- (void)setFormatDescription:(CMVideoFormatDescriptionRef)formatDescription {
    if (!CMFormatDescriptionEqual(formatDescription, _formatDescription)) {
        [self freeFormatDescription];
        _formatDescription = formatDescription;
        [self createDecompressionSessionIfRequired:formatDescription];
    }
}

@end
