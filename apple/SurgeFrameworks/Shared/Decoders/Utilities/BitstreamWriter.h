// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

/**
 * A wrapper around primitive operations for writing bitstreams.
 */
@interface BitstreamWriter : NSObject

- (void)writeBit:(uint8_t)value;

- (void)writeByte:(uint8_t)value;

- (void)writeTwoBytes:(uint16_t)value;

- (void)writeThreeBytes:(uint32_t)value;

- (void)writeFourBytes:(uint32_t)value;

- (void)writeBytes:(NSData*)data;

- (void)writeBits:(uint8_t)value numberOfBits:(uint8_t)numberOfBits;

- (NSData*)data;

@end
