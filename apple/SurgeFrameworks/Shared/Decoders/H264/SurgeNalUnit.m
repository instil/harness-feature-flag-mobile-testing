// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

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
