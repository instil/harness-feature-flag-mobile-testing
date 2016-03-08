//
//  NaluSegment.h
//  H264DecoderWrapper
//
//  Created by Philip Herron on 24/08/2015.
//  Copyright (c) 2015 Instil Software. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NaluSegmentTypes.h"

@interface NaluSegment : NSObject

@property (nonatomic, assign) size_t headerSize;
@property (nonatomic, assign) size_t offset;
@property (nonatomic, assign) NaluSegmentType type;
@property (nonatomic, assign) size_t length;

- (id)initWithType:(NaluSegmentType)type atOffset:(size_t)offset withHeaderSize:(size_t)headerSize;

- (void)setBuffer:(const unsigned char *)buffer withLength:(size_t)length;

- (void*)annexBBuffer;

- (void*)bufferWithoutHeader;

- (void)cleanup;

@end
