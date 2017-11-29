// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "BitstreamWriter.h"

@interface BitstreamWriter ()
@property (nonatomic, assign) uint8_t *buffer;
@property (nonatomic, assign) uint32_t currentBitOffset;
@end

@implementation BitstreamWriter

- (instancetype)init {
    self = [super init];
    if (self) {
        self.buffer = calloc(1024, sizeof(uint8_t));
    }
    return self;
}

- (void)dealloc {
    free(self.buffer);
}

- (void)writeBit:(uint8_t)value {
    [self writeBits:value numberOfBits:1];
}

- (void)writeByte:(uint8_t)value {
    [self writeBits:value numberOfBits:8];
}

- (void)writeTwoBytes:(uint16_t)value {
    [self writeByte:(value >> 8)];
    [self writeByte:value];
}

- (void)writeThreeBytes:(uint32_t)value {
    [self writeByte:(value >> 16)];
    [self writeByte:(value >> 8)];
    [self writeByte:value];
}

- (void)writeFourBytes:(uint32_t)value {
    [self writeByte:(value >> 24)];
    [self writeByte:(value >> 16)];
    [self writeByte:(value >> 8)];
    [self writeByte:value];
}

- (void)writeBytes:(NSData*)data {
    uint8_t *bytes = (uint8_t *)[data bytes];
    for (int i = 0; i < [data length]; i++) {
        [self writeByte:bytes[i]];
    }
}

- (void)writeBits:(uint8_t)value numberOfBits:(uint8_t)numberOfBits {
    uint32_t byteOffset = self.currentBitOffset / 8;
    uint8_t bitOffset = self.currentBitOffset % 8;
    uint8_t maxBitsWritable = 8 - bitOffset;
    if (numberOfBits <= maxBitsWritable) {
        self.buffer[byteOffset] |= value << (maxBitsWritable - numberOfBits);
    } else {
        self.buffer[byteOffset] |= value >> bitOffset;
        self.buffer[byteOffset + 1] = value << (8 - bitOffset);
    }
    self.currentBitOffset += numberOfBits;
}

- (NSData*)data {
    return [NSData dataWithBytes:self.buffer length:self.currentBitOffset / 8];
}

@end
