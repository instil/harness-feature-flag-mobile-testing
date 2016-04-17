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

@interface SurgeMp4vDecoder ()
@property (nonatomic, assign) CMVideoFormatDescriptionRef formatDesc;
@property (nonatomic, assign) VTDecompressionSessionRef decompressionSession;

//@property (nonatomic, assign, getter=isRunning) BOOL running;
@property (nonatomic, strong) dispatch_queue_t decoderQueue;
@property (nonatomic, strong) NSMutableArray *decoderSampleQueue;
@property (nonatomic, strong) dispatch_semaphore_t sema;
@property (nonatomic, strong) dispatch_semaphore_t semaQueue;

@property (nonatomic, assign) NSTimeInterval timeStampForLastDecodedFrame;
@property (nonatomic, assign) int framePerSecondCounter;
@property (nonatomic, assign) NSTimeInterval timeSinceLastFpsNotifaction;
@end

@implementation SurgeMp4vDecoder

- (void)decodeFrameBuffer:(const unsigned char*)frameBuffer
                   ofSize:(size_t)size
        withFrameDuration:(int)frameDuration
      andPresentationTime:(unsigned int)presentationTimeInterval {
    
    
    
}

//- (void)createSampleBuffer {
//    
//}



@end
