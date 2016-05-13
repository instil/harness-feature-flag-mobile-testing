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
    self.currentBitOffset += bitsToRead;
    
    NSAssert(byteOffset < self.size, @"Buffer has been completely consumed");
    uint32_t value = (*(self.buffer + byteOffset) >> (8 - bitsToRead - bitOffset));
    
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
