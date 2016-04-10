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
