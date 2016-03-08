//
//  NaluParser.h
//  H264DecoderWrapper
//
//  Created by Philip Herron on 24/08/2015.
//  Copyright (c) 2015 Instil Software. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NaluParser : NSObject

+ (BOOL)segmentList:(NSArray*)nalus containsNalu:(NSInteger)type;

+ (NSArray*)findNalusOfType:(NSInteger)type inArray:(NSArray*)nalus;

+ (NSArray*)parseOutNalusFromFrameBuffer:(const unsigned char *)frameBuffer ofLength:(size_t)frameSize;

@end
