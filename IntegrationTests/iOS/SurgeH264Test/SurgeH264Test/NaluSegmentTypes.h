//
//  NaluSegmentTypes.h
//  H264DecoderWrapper
//
//  Created by Niall Kelly on 25/09/2015.
//  Copyright © 2015 Instil. All rights reserved.
//

#ifndef NaluSegmentTypes_h
#define NaluSegmentTypes_h

typedef unsigned char byte;
#define ZERO_BYTE  (byte)0x00
#define MAGIC_BYTE (byte)0x01

typedef NS_ENUM(NSUInteger, NaluSegmentType) {
    NaluSegmentTypeUnspecified = 0,
    NaluSegmentTypeNonIDR = 1,
    NaluSegmentTypePartitionA = 2,
    NaluSegmentTypePartitionB = 3,
    NaluSegmentTypePartitionC = 4,
    NaluSegmentTypeIDR = 5,
    NaluSegmentTypeSEI = 6,
    NaluSegmentTypeSPS = 7,
    NaluSegmentTypePPS = 8,
    NaluSegmentTypeAUD = 9,
    NaluSegmentTypeEndOfSequence = 10,
    NaluSegmentTypeEndOfStream = 11,
    NaluSegmentTypeFilterData = 12,
    NaluSegmentTypeSPSE = 13,
    NaluSegmentTypePrefixNALUnit = 14,
    NaluSegmentTypeSubSPS = 15,
    NaluSegmentTypeReserved1 = 16,
    NaluSegmentTypeReserved2 = 17,
    NaluSegmentTypeReserved3 = 18,
    NaluSegmentTypeAuxilarySliceWithoutParitioning = 19,
    NaluSegmentTypeCSE = 20,
    NaluSegmentTypeCSEDepthViewComponents = 21,
    NaluSegmentTypeReserved4 = 22,
    NaluSegmentTypeReserved5 = 23,
    NaluSegmentTypeSTAPA = 24,
    NaluSegmentTypeSTAPB = 25,
    NaluSegmentTypeMTAP16 = 26,
    NaluSegmentTypeMTAP24 = 27,
    NaluSegmentTypeFUA = 28,
    NaluSegmentTypeFUB = 29,
    NaluSegmentTypeUnspecified1 = 30,
    NaluSegmentTypeUnspecified2 = 31
};

#endif /* NaluSegmentTypes_h */
