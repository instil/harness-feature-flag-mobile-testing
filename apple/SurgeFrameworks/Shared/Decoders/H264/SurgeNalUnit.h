// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

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
