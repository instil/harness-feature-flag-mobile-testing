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

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <VideoToolbox/VideoToolbox.h>

#import "SurgeH264Decoder.h"
#import "NaluParser.h"
#import "NaluSegment.h"
#import "NaluSegmentTypes.h"
#import "SurgeLogging.h"

@interface DecoderSample : NSObject
@property (nonatomic, assign) CMSampleBufferRef sampleBuffer;
@property (nonatomic, assign) CMBlockBufferRef blockBuffer;
@property (nonatomic, assign) CMFormatDescriptionRef formatDescription;
@property (nonatomic, assign) int frameDuration;
@property (nonatomic, assign) double presentationTime;
@end

@implementation DecoderSample
@end

@interface SurgeH264Decoder ()
@property (nonatomic, assign) CMVideoFormatDescriptionRef formatDesc;
@property (nonatomic, assign) VTDecompressionSessionRef decompressionSession;

@property (nonatomic, assign, getter=isRunning) BOOL running;
@property (nonatomic, strong) dispatch_queue_t decoderQueue;
@property (nonatomic, strong) NSMutableArray *decoderSampleQueue;
@property (nonatomic, strong) dispatch_semaphore_t sema;
@property (nonatomic, strong) dispatch_semaphore_t semaQueue;

@property (nonatomic, assign) NSTimeInterval timeStampForLastDecodedFrame;
@property (nonatomic, assign) int framePerSecondCounter;
@property (nonatomic, assign) NSTimeInterval timeSinceLastFpsNotifaction;
@end

@implementation SurgeH264Decoder

- (id)initWithDelegate:(id<SurgeDecoderDelegate>)delegate {
    if (self = [super init]) {
        [self setDelegate:delegate];
        [self setSema:dispatch_semaphore_create(1)];
        [self setSemaQueue:dispatch_semaphore_create(0)];
        [self resetDecoder];
        [self setDecoderQueue:dispatch_queue_create("co.instil.h264.decoder", DISPATCH_QUEUE_SERIAL)];
        [self startDecoder];
    }
    return self;
}

#pragma mark - Dealloc

- (void)dealloc {
    [self resetDecoder];
    [self stopDecoder];
}

- (void)resetDecoder {
    [self acquireLock];
    [self setFramePerSecondCounter:0];
    [self setTimeStampForLastDecodedFrame:0];
    [self freeFormatDescription];
    [self freeVideoDecompressionSession];
    [self resetDecoderSampleQueue];
    [self releaseLock];
}

- (void)resetDecoderSampleQueue {
    for (DecoderSample *decodeSample in self.decoderSampleQueue) {
        CFRelease(decodeSample.blockBuffer);
        CFRelease(decodeSample.sampleBuffer);
    }
    [self setSemaQueue:dispatch_semaphore_create(0)];
    [self setDecoderSampleQueue:[NSMutableArray new]];
}

- (void)freeFormatDescription {
    if (_formatDesc != NULL) {
        CFRelease(_formatDesc);
        _formatDesc = NULL;
    }
}

- (void)freeVideoDecompressionSession {
    if (_decompressionSession) {
        VTDecompressionSessionInvalidate(_decompressionSession);
        CFRelease(_decompressionSession);
        _decompressionSession = NULL;
    }
}

#pragma mark - Decoder

- (void)decodeFrameBuffer:(const unsigned char*)frameBuffer
                   ofSize:(size_t)size
        withFrameDuration:(int)frameDuration
      andPresentationTime:(unsigned int)presentationTimeInterval {

    if ([self timeoutSinceLastDecodedFrameExceeded]) {
        [self notifyDelegateBuffering];
    }
    
    NSArray *nalus = [NaluParser parseOutNalusFromFrameBuffer:frameBuffer ofLength:size];
    for (NaluSegment *segment in nalus) {
        SurgeLogDebug(@"NALU Type : %lu", (unsigned long)segment.type);
    }
    
    OSStatus status = noErr;
    if ([NaluParser segmentList:nalus containsNalu:NaluSegmentTypeSPS] && [NaluParser segmentList:nalus containsNalu:NaluSegmentTypePPS]) {
        
        CMFormatDescriptionRef formatDescription;
        status = [self setupVideoFormatDescription:&formatDescription fromNalus:nalus];
        
        if (status == noErr) {
            [self acquireLock];
            if (_decompressionSession == NULL) {
                _formatDesc = formatDescription;
                [self createDecompressionSession];
            } else {
                BOOL needNewDecompSession = (VTDecompressionSessionCanAcceptFormatDescription(_decompressionSession, formatDescription) == NO);
                if (needNewDecompSession) {
                    [self freeFormatDescription];
                    _formatDesc = formatDescription;
                    [self createDecompressionSession];
                } else {
                    [self freeFormatDescription];
                    _formatDesc = formatDescription;
                }
            }
            [self releaseLock];
        }
    }
    
    size_t blockLength = 0;
    CMBlockBufferRef blockBuffer = NULL;
    BOOL foundIdrFrame = [NaluParser segmentList:nalus containsNalu:NaluSegmentTypeIDR];
    BOOL foundNonIdrFrame = [NaluParser segmentList:nalus containsNalu:NaluSegmentTypeNonIDR];
    if (foundIdrFrame || foundNonIdrFrame) {
        NaluSegmentType segmentType = (foundIdrFrame) ? NaluSegmentTypeIDR : NaluSegmentTypeNonIDR;
        
        NSArray *idrs = [NaluParser findNalusOfType:segmentType inArray:nalus];
        NaluSegment *nalu = [idrs lastObject];
        blockLength = nalu.length;
        status = [self setupBlockBuffer:&blockBuffer withSegment:nalu];
    }
    
    // Create and render the sample buffer
    if (status == noErr && blockBuffer != NULL) {
        [self acquireLock];
        CMSampleBufferRef sampleBuffer = NULL;
        [self setupSampleBuffer:&sampleBuffer
                  toBlockBuffer:&blockBuffer
                       ofLength:blockLength
             andNumberOfSamples:1
           forFormatDescription:self.formatDesc
              withFrameDuration:frameDuration
            andPresentationTime:presentationTimeInterval];
        
        DecoderSample *sample = [[DecoderSample alloc] init];
        sample.sampleBuffer = sampleBuffer;
        sample.blockBuffer = blockBuffer;
        sample.formatDescription = self.formatDesc;
        sample.frameDuration = frameDuration;
        sample.presentationTime = presentationTimeInterval;
        
        [self.decoderSampleQueue addObject:sample];
        [self releaseSampleResource];
        [self releaseLock];
    }
    
    // Make sure to cleanup all buffers within nalu segments.
    for (NaluSegment *segment in nalus) {
        [segment cleanup];
    }
}

- (void)startDecoder {
    if (self.isRunning) {
        return;
    }
    self.running = YES;
    dispatch_async(self.decoderQueue, ^{
        while(self.isRunning) {
            [self acquireSampleResource];
            [self acquireLock];
            BOOL isReady = (self.decompressionSession != NULL && self.formatDesc != NULL && self.decoderSampleQueue.count > 0);
            if (self.isRunning && isReady) {
                DecoderSample *lastSample = [self.decoderSampleQueue objectAtIndex:0];
                [self.decoderSampleQueue removeObjectAtIndex:0];
                
                // because this is async we need to ensure our format descriptions match
                // referencing old/invalid format description will cause a crash
                if (CMFormatDescriptionEqual(lastSample.formatDescription, _formatDesc)) {
                    [self decodeSampleBuffer:lastSample.sampleBuffer];
                }
                
                // release the last sample
                CFRelease(lastSample.blockBuffer);
                CFRelease(lastSample.sampleBuffer);
            }
            [self releaseLock];
            [self calculateFramesPerSecond];
        }
    });
}

- (void)stopDecoder {
    // stop the decoder loop
    [self setRunning:NO];
    // to ensure the decoder loop doesn't block;
    [self releaseSampleResource];
    // ensures it is stopped AKA join - works because this is a serial queue
    dispatch_sync(self.decoderQueue, ^{});
    // reset ready for play
    [self resetDecoder];
}

#pragma mark - Queue Lock

- (void)acquireLock {
    dispatch_semaphore_wait(self.sema, DISPATCH_TIME_FOREVER);
}

- (void)releaseLock {
    dispatch_semaphore_signal(self.sema);
}

#pragma mark - Resource sema

- (void)acquireSampleResource {
    dispatch_semaphore_wait(self.semaQueue, DISPATCH_TIME_FOREVER);
}

- (void)releaseSampleResource {
    dispatch_semaphore_signal(self.semaQueue);
}

#pragma mark - Decoder Setup - Render

- (OSStatus)decodeSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    VTDecodeFrameFlags decodeFlags = (kVTDecodeFrame_EnableAsynchronousDecompression);
    return VTDecompressionSessionDecodeFrame(self.decompressionSession,
                                             sampleBuffer,
                                             decodeFlags,
                                             NULL,
                                             0);
}

#pragma mark - Setup Sample Buffer

- (OSStatus)setupSampleBuffer:(CMSampleBufferRef*)sampleBuffer
                toBlockBuffer:(CMBlockBufferRef*)blockBuffer
                     ofLength:(size_t)blockLength
           andNumberOfSamples:(CMItemCount)numberOfSamples
         forFormatDescription:(CMFormatDescriptionRef)formatDescription
            withFrameDuration:(int)frameDuration
          andPresentationTime:(unsigned int)presentationTime {
    CMSampleTimingInfo timingInfo;
    timingInfo.duration = CMTimeMakeWithSeconds(frameDuration, 1);
    timingInfo.decodeTimeStamp = kCMTimeInvalid;
    timingInfo.presentationTimeStamp = CMTimeMakeWithSeconds(presentationTime, 1);
    
    return CMSampleBufferCreate(kCFAllocatorDefault,
                                *blockBuffer,
                                true,
                                NULL,
                                NULL,
                                formatDescription,
                                numberOfSamples,
                                1,
                                &timingInfo,
                                numberOfSamples,
                                &blockLength,
                                sampleBuffer);
}

#pragma mark - Decoder Setup - Create Block Buffer

- (OSStatus)setupBlockBuffer:(CMBlockBufferRef*)buffer withSegment:(NaluSegment*)segment {
    void *origionalData = [segment annexBBuffer];
    void *data = malloc(segment.length);
    memcpy(data, origionalData, segment.length);
    
    // replace the start code header on this NALU with its size.
    // AVCC format requires that you do this.
    // htonl converts the unsigned int from host to network byte order
    uint32_t dataLength32 = htonl(segment.length - 4);
    memcpy(data, &dataLength32, 4);
    
    // create a block buffer from the IDR NALU
    OSStatus status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                         data,                // memoryBlock to hold buffered data
                                                         segment.length,      // block length of the mem block in bytes.
                                                         kCFAllocatorMalloc,
                                                         NULL,
                                                         0,                   // offsetToData
                                                         segment.length,      // dataLength of relevant bytes, starting at offsetToData
                                                         0,
                                                         buffer);
    return status;
}

#pragma mark - Decoder Setup - Format Description

- (OSStatus)setupVideoFormatDescription:(CMFormatDescriptionRef*)formatDescription fromNalus:(NSArray*)nalus {
    NSArray *pictureParamSet = [NaluParser findNalusOfType:NaluSegmentTypePPS inArray:nalus];
    NSArray *sequenceParamSet = [NaluParser findNalusOfType:NaluSegmentTypeSPS inArray:nalus];
    
    size_t totalNumberOfParameters = 2;
    uint8_t **parameterSetPointers = calloc(totalNumberOfParameters, sizeof(uint8_t*));
    size_t *parameterSetSizes = calloc(totalNumberOfParameters, sizeof(size_t));
    
    NaluSegment *ppsSegment = pictureParamSet.lastObject;
    parameterSetSizes[0] = ppsSegment.length - ppsSegment.headerSize;
    parameterSetPointers[0] = [ppsSegment bufferWithoutHeader];
    
    NaluSegment *spsSegment = sequenceParamSet.lastObject;
    parameterSetSizes[1] = spsSegment.length - spsSegment.headerSize;
    parameterSetPointers[1] = [spsSegment bufferWithoutHeader];
    
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

#pragma mark - Decompression Session

- (void)renderH264Sample:(CVImageBufferRef)imageBuffer withPresentationTime:(CMTime)presentationTime {
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

void decompressionSessionDecodeFrameCallback(void *decompressionOutputRefCon,
                                             void *sourceFrameRefCon,
                                             OSStatus status,
                                             VTDecodeInfoFlags infoFlags,
                                             CVImageBufferRef imageBuffer,
                                             CMTime presentationTimeStamp,
                                             CMTime presentationDuration) {
    
    SurgeH264Decoder *decoder = (__bridge SurgeH264Decoder *)decompressionOutputRefCon;
    if (status == noErr) {
        decoder.framePerSecondCounter++;
        [decoder renderH264Sample:imageBuffer withPresentationTime:presentationTimeStamp];
        if ([decoder timeoutSinceLastDecodedFrameExceeded]) {
            [decoder notifyDelegateStoppedBuffering];
        }
        decoder.timeStampForLastDecodedFrame = [decoder currentTimeStamp];
    } else {
        SurgeLogError(@"Decode failed: %i", status);
    }
}

- (void)createDecompressionSession {
    if (_decompressionSession != NULL) {
        VTDecompressionSessionInvalidate(self.decompressionSession);
        CFRelease(self.decompressionSession);
    }
    
    VTDecompressionOutputCallbackRecord callBackRecord;
    callBackRecord.decompressionOutputCallback = decompressionSessionDecodeFrameCallback;
    // this is necessary if you need to make calls to Objective C "self" from within in the callback method.
    callBackRecord.decompressionOutputRefCon = (__bridge void *)self;
    
    // you can set some desired attributes for the destination pixel buffer.  I didn't use this but you may
    // if you need to set some attributes, be sure to uncomment the dictionary in VTDecompressionSessionCreate
    NSDictionary *destinationImageBufferAttributes = @{ (id)kCVPixelBufferOpenGLESCompatibilityKey: @YES,
                                                        (id)kCVPixelBufferCGImageCompatibilityKey: @YES };
    
    OSStatus status =  VTDecompressionSessionCreate(NULL,
                                                    _formatDesc,
                                                    NULL,
                                                    (__bridge CFDictionaryRef)(destinationImageBufferAttributes),
                                                    &callBackRecord,
                                                    &_decompressionSession);
    SurgeLogDebug(@"Video Decompression Session Create: \t %@", (status == noErr) ? @"successful!" : @"failed...");
    if(status != noErr) {
        SurgeLogError(@"\t\t VTD ERROR type: %d", (int)status);
    }
}

#pragma mark - FPS

- (void)calculateFramesPerSecond {
    NSTimeInterval diff = self.timeStampForLastDecodedFrame - self.timeSinceLastFpsNotifaction;
    if (diff >= 1) {
        self.framesPerSecond = self.framePerSecondCounter;
        self.framePerSecondCounter = 0;
        self.timeSinceLastFpsNotifaction = self.timeStampForLastDecodedFrame;
    }
}

#pragma mark - TimeStamp

- (NSTimeInterval)currentTimeStamp {
    return [NSDate timeIntervalSinceReferenceDate];
}

#pragma mark - Timeout

- (BOOL)timeoutSinceLastDecodedFrameExceeded {
    return ([self currentTimeStamp] - self.timeStampForLastDecodedFrame) >= 5;
}

#pragma mark - Notify Delegate

- (void)notifyDelegateBuffering {
    if ([self.delegate respondsToSelector:@selector(decoderDidBeginBuffering)]) {
        [self.delegate decoderDidBeginBuffering];
    }
}

- (void)notifyDelegateStoppedBuffering {
    if ([self.delegate respondsToSelector:@selector(decoderDidStopBuffering)]) {
        [self.delegate decoderDidStopBuffering];
    }
}

@end
