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
@property (nonatomic, assign) CMFormatDescriptionRef formatDescription;
@end

@implementation SurgeMp4vDecoder

- (void)decodeFrameBuffer:(const unsigned char*)frameBuffer
                   ofSize:(size_t)size
        withFrameDuration:(int)frameDuration
      andPresentationTime:(unsigned int)presentationTimeInterval {
    
    if (self.formatDescription == NULL) {
        CMFormatDescriptionRef formatDescription;
        OSStatus status = CMVideoFormatDescriptionCreate(kCFAllocatorDefault, kCMVideoCodecType_MPEG4Video, 720, 480, NULL, &formatDescription);
        if (status == noErr) {
            self.formatDescription = formatDescription;
        } else {
            SurgeLogError(@"Failed to create MP4V-ES format description: %d", status);
            return;
        }
    }
    
    CMBlockBufferRef blockBuffer = NULL;
    OSStatus status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                         frameBuffer,
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


@end
