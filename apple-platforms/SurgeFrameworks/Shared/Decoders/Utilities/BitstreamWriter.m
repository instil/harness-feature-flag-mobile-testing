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
    uint8_t *bytes = [data bytes];
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
