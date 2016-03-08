//
//  H264DecoderDelegate.h
//  H264DecoderWrapper
//
//  Created by Philip Herron on 24/08/2015.
//  Copyright (c) 2015 Instil Software. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreImage/CoreImage.h>

@protocol H264DecoderDelegate <NSObject>

- (void)decodedFrame:(CGImageRef)imageBuffer withTimeStamp:(double)timestamp;

@optional

- (void)decoderIsBuffering;

- (void)decoderStoppedBuffering;

- (void)framesPerSecond:(int)fps;

@end
