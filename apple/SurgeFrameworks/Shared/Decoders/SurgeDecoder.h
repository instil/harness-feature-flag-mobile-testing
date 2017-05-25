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
#import <CoreMedia/CoreMedia.h>
#import <VideoToolbox/VideoToolbox.h>

@protocol SurgeDecoderDelegate <NSObject>

/**
 * Called when a decoded frame is available for
 * presentation at the application layer.
 */
- (void)decoderFrameAvailable:(CGImageRef)image withTimeStamp:(NSTimeInterval)timestamp;

- (void)decoderFramerateUpdated:(NSInteger) framerate;

@end


@interface SurgeDecoder : NSObject

- (id)initWithDelegate:(id<SurgeDecoderDelegate>)delegate;

- (void)deinit;

/**
 * Abstract method to be implemented by decoders. Implementations
 * should create a CMSampleBufferRef and enqueue for decoding.
 */
- (void)decodeFrameBuffer:(const uint8_t *)frameBuffer
                   ofSize:(size_t)size
           withDimensions:(CGSize)dimensions
         presentationTime:(unsigned int)presentationTime
                 duration:(int)duration;

/**
 * Enqueue a CMSampleBufferRef for decoding. Once decoded, the frame
 * will be made available through a delegate callback as a CGImageRef.
 */
- (void)enqueueSampleBuffer:(CMSampleBufferRef)sampleBuffer;

@property (nonatomic, weak) id<SurgeDecoderDelegate> delegate;

@property (nonatomic, assign) NSUInteger framesPerSecond;

@end
