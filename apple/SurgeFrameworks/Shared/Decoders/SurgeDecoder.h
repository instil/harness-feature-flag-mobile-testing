// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <VideoToolbox/VideoToolbox.h>
#import "DiagnosticsTracker.h"
#import "SurgeMediaFormat.h"

@protocol SurgeDecoderDelegate <NSObject>

/**
 * Called when a decoded frame is available for
 * presentation at the application layer.
 */
- (void)decoderFrameAvailable:(CGImageRef)image withTimeStamp:(NSTimeInterval)timestamp;

@end


@interface SurgeDecoder : NSObject

- (id)initWithDelegate:(id<SurgeDecoderDelegate>)delegate andDiagnosticsTracker:(DiagnosticsTracker *)diagnosticsTracker;

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
@property (nonatomic, strong) DiagnosticsTracker *diagnostics;

@property (nonatomic, assign) CMVideoFormatDescriptionRef formatDescription;
@property (nonatomic, assign) NSUInteger framesPerSecond;

@property (nonatomic, readonly) SurgeMediaFormat mediaFormat;

@end
