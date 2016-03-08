//
//  H264DecoderWrapper.h
//  H264DecoderWrapper
//
//  Created by Philip Herron on 03/08/2015.
//  Copyright (c) 2015 Instil Software. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <AVFoundation/AVFoundation.h>
#import <VideoToolbox/VideoToolbox.h>

#import "H264DecoderDelegate.h"

@interface H264DecoderWrapper : NSObject

@property (nonatomic, weak) id<H264DecoderDelegate> delegate;

@property (nonatomic, assign, readonly, getter=isBuffering) BOOL buffering;

@property (nonatomic, assign, readonly, getter=isRunning) BOOL running;

- (id)initWithDelegate:(id<H264DecoderDelegate>)delegate;

- (void)decodeFrameBuffer:(const char*)frameBuffer
                   ofSize:(size_t)size
        withFrameDuration:(int)frameDuration
      andPresentationTime:(unsigned int)presentationTimeInterval;

- (void)startDecoder;

- (void)stopDecoder;

@end
