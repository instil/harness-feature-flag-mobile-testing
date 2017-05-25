// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import "BitstreamReader.h"

@interface BitstreamReader ()
@property (nonatomic, assign) uint8_t *buffer;
@property (nonatomic, assign) size_t size;
@property (nonatomic, assign) uint32_t currentBitOffset;
@end

@implementation BitstreamReader

- (instancetype)initWithBuffer:(uint8_t*)buffer ofSize:(size_t)size {
    self = [super init];
    if (self) {
        self.buffer = buffer;
        self.size = size;
        self.currentBitOffset = 0;
    }
    return self;
}

- (uint8_t)readBit {
    return [self readNumberOfBits:1];
}

- (uint8_t)readByte {
    return [self readNumberOfBits:8];
}

- (uint16_t)readTwoBytes {
    return [self readNumberOfBits:16];
}

- (uint32_t)readFourBytes {
    return [self readNumberOfBits:32];
}

- (uint32_t)readNumberOfBits:(uint8_t)numberOfBits {
    uint32_t byteOffset = self.currentBitOffset / 8;
    uint8_t bitOffset = self.currentBitOffset % 8;
    uint8_t maxBitsReadable = 8 - bitOffset;
    uint8_t bitsToRead = numberOfBits <= maxBitsReadable ? numberOfBits : maxBitsReadable;
    
    NSAssert(byteOffset < self.size, @"Buffer has been completely consumed");
    uint8_t mask = pow(2, bitsToRead) - 1;
    uint32_t value = (self.buffer[byteOffset] >> (8 - bitsToRead - bitOffset)) & mask;
    self.currentBitOffset += bitsToRead;
    
    uint8_t bitsRemaining = numberOfBits - bitsToRead;
    if (bitsRemaining > 0) {
        value = (value << bitsRemaining) | [self readNumberOfBits:bitsRemaining];
    }

    return value;
}

- (void)skipNumberOfBits:(uint32_t)numberOfBits {
    self.currentBitOffset += numberOfBits;
}

@end
