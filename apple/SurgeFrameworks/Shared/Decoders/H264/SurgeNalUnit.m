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

#import "SurgeNalUnit.h"

@interface SurgeNalUnit ()
@property (nonatomic, assign) unsigned char *buffer;
@end

@implementation SurgeNalUnit

- (id)initWithType:(SurgeNalUnitType)type atOffset:(size_t)offset withHeaderSize:(size_t)headerSize {
    if (self = [super init]) {
        [self setType:type];
        [self setOffset:offset];
        [self setHeaderSize:headerSize];
    }
    return self;
}

#pragma mark - Lifecycle

- (void)dealloc {
    [self cleanup];
}

- (void)cleanup {
    if (self.buffer) {
        free(self.buffer);
        [self setBuffer:nil];
    }
}

#pragma mark - Buffer copy

- (void*)annexBBuffer {
    return self.buffer;
}

- (void*)bufferWithoutHeader {
    return self.buffer + self.headerSize;
}

#pragma mark - Fix the buffer

// makes a copy
- (void)setBuffer:(const unsigned char*)buffer withLength:(size_t)length {
    
    if ((byte)buffer[0] == ZERO_BYTE && (byte)buffer[1] == ZERO_BYTE && (byte)buffer[2] == MAGIC_BYTE) {
        self.buffer = malloc(length+1);
        [self setLength:length + 1];
        [self setHeaderSize:4];
        memset(self.buffer, 0, length + 1);
        memcpy(self.buffer + 1, buffer, length);
    } else if (buffer[0] == ZERO_BYTE && buffer[1] == ZERO_BYTE && buffer[2] == ZERO_BYTE && buffer[3] == MAGIC_BYTE) {
        self.buffer = malloc(length);
        [self setLength:length];
        [self setHeaderSize:4];
        memcpy(self.buffer, buffer, length);
    } else {
        @throw [NSException exceptionWithName:@"SurgeNalUnitException"
                                       reason:@"NAL unit contains invalid magic-byte sequence"
                                     userInfo:nil];
    }
}

@end