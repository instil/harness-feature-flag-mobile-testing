// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

/**
 * A wrapper around primitive operations for reading bitstreams.
 */
@interface BitstreamReader : NSObject

- (instancetype)initWithBuffer:(uint8_t*)buffer ofSize:(size_t)size;

- (uint8_t)readBit;

- (uint8_t)readByte;

- (uint16_t)readTwoBytes;

- (uint32_t)readFourBytes;

- (uint32_t)readNumberOfBits:(uint8_t)numberOfBits;

- (void)skipNumberOfBits:(uint32_t)numberOfBits;

@end
