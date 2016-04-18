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

#import <Foundation/Foundation.h>

typedef unsigned char byte;
#define ZERO_BYTE  (byte)0x00
#define MAGIC_BYTE (byte)0x01

typedef NS_ENUM(NSUInteger, SurgeNalUnitType) {
    SurgeNalUnitTypeUnspecified = 0,
    SurgeNalUnitTypeNonIDR = 1,
    SurgeNalUnitTypePartitionA = 2,
    SurgeNalUnitTypePartitionB = 3,
    SurgeNalUnitTypePartitionC = 4,
    SurgeNalUnitTypeIDR = 5,
    SurgeNalUnitTypeSEI = 6,
    SurgeNalUnitTypeSPS = 7,
    SurgeNalUnitTypePPS = 8,
    SurgeNalUnitTypeAUD = 9,
    SurgeNalUnitTypeEndOfSequence = 10,
    SurgeNalUnitTypeEndOfStream = 11,
    SurgeNalUnitTypeFilterData = 12,
    SurgeNalUnitTypeSPSE = 13,
    SurgeNalUnitTypePrefixNALUnit = 14,
    SurgeNalUnitTypeSubSPS = 15,
    SurgeNalUnitTypeReserved1 = 16,
    SurgeNalUnitTypeReserved2 = 17,
    SurgeNalUnitTypeReserved3 = 18,
    SurgeNalUnitTypeAuxilarySliceWithoutParitioning = 19,
    SurgeNalUnitTypeCSE = 20,
    SurgeNalUnitTypeCSEDepthViewComponents = 21,
    SurgeNalUnitTypeReserved4 = 22,
    SurgeNalUnitTypeReserved5 = 23,
    SurgeNalUnitTypeSTAPA = 24,
    SurgeNalUnitTypeSTAPB = 25,
    SurgeNalUnitTypeMTAP16 = 26,
    SurgeNalUnitTypeMTAP24 = 27,
    SurgeNalUnitTypeFUA = 28,
    SurgeNalUnitTypeFUB = 29,
    SurgeNalUnitTypeUnspecified1 = 30,
    SurgeNalUnitTypeUnspecified2 = 31
};

@interface SurgeNalUnit : NSObject

@property (nonatomic, assign) size_t headerSize;
@property (nonatomic, assign) size_t offset;
@property (nonatomic, assign) SurgeNalUnitType type;
@property (nonatomic, assign) size_t length;

- (id)initWithType:(SurgeNalUnitType)type atOffset:(size_t)offset withHeaderSize:(size_t)headerSize;

- (void)setBuffer:(const unsigned char *)buffer withLength:(size_t)length;

- (void*)annexBBuffer;

- (void*)bufferWithoutHeader;

- (void)cleanup;

@end
