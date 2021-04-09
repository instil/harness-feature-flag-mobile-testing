//
//  SurgeH265NalUnit.h
//  SurgeiOS
//
//  Created by Paul Shields on 05/11/2020.
//

#import <Foundation/Foundation.h>

typedef unsigned char byte;
#define ZERO_BYTE  (byte)0x00
#define MAGIC_BYTE (byte)0x01

typedef NS_ENUM(NSUInteger, SurgeH265NalUnitType) {
    SurgeH265NalUnitCodedSliceTrailN = 0,   // 0
    SurgeH265NalUnitCodedSliceTrialR,   // 1

    SurgeH265NalUnitCodedSliceTsaN,     // 2
    SurgeH265NalUnitCodedSliceTla,       // 3   // Current name in the spec: TSA_R

    SurgeH265NalUnitCodedSliceStsaN,    // 4
    SurgeH265NalUnitCodedSliceStsaR,    // 5

    SurgeH265NalUnitCodedSliceRadlN,    // 6
    SurgeH265NalUnitCodedSliceDlp,       // 7 // Current name in the spec: RADL_R

    SurgeH265NalUnitCodedSliceRaslN,    // 8
    SurgeH265NalUnitCodedSliceTfd,       // 9 // Current name in the spec: RASL_R

    SurgeH265NalUnitReserved10,
    SurgeH265NalUnitReserved11,
    SurgeH265NalUnitReserved12,
    SurgeH265NalUnitReserved13,
    SurgeH265NalUnitReserved14,
    SurgeH265NalUnitReserved15,
    SurgeH265NalUnitCodedSliceBla,       // 16   // Current name in the spec: BLA_W_LP
    SurgeH265NalUnitCodedSliceBlant,     // 17   // Current name in the spec: BLA_W_DLP
    SurgeH265NalUnitCodedSliceBlaNLP,  // 18
    SurgeH265NalUnitCodedSliceIdr,       // 19  // Current name in the spec: IDR_W_DLP
    SurgeH265NalUnitCodedSliceIdrNLp,  // 20
    SurgeH265NalUnitCodedSliceCra,       // 21
    SurgeH265NalUnitReserved22,
    SurgeH265NalUnitReserved23,

    SurgeH265NalUnitReserved24,
    SurgeH265NalUnitReserved25,
    SurgeH265NalUnitReserved26,
    SurgeH265NalUnitReserved27,
    SurgeH265NalUnitReserved28,
    SurgeH265NalUnitReserved29,
    SurgeH265NalUnitReserved30,
    SurgeH265NalUnitReserved31,

    SurgeH265NalUnitVPS,                   // 32
    SurgeH265NalUnitSPS,                   // 33
    SurgeH265NalUnitPPS,                   // 34
    SurgeH265NalUnitAccessUnitDelimiter, // 35
    SurgeH265NalUnitEOS,                   // 36
    SurgeH265NalUnitEOB,                   // 37
    SurgeH265NalUnitFillerData,           // 38
    SurgeH265NalUnitSEI,                   // 39 Prefix SEI
    SurgeH265NalUnitSEISuffix,            // 40 Suffix SEI
    SurgeH265NalUnitReserved41,
    SurgeH265NalUnitReserved42,
    SurgeH265NalUnitReserved43,
    SurgeH265NalUnitReserved44,
    SurgeH265NalUnitReserved45,
    SurgeH265NalUnitReserved46,
    SurgeH265NalUnitReserved47,
    SurgeH265NalUnitUnspecified48,
    SurgeH265NalUnitUnspecified49,
    SurgeH265NalUnitUnspecified50,
    SurgeH265NalUnitUnspecified51,
    SurgeH265NalUnitUnspecified52,
    SurgeH265NalUnitUnspecified53,
    SurgeH265NalUnitUnspecified54,
    SurgeH265NalUnitUnspecified55,
    SurgeH265NalUnitUnspecified56,
    SurgeH265NalUnitUnspecified57,
    SurgeH265NalUnitUnspecified58,
    SurgeH265NalUnitUnspecified59,
    SurgeH265NalUnitUnspecified60,
    SurgeH265NalUnitUnspecified61,
    SurgeH265NalUnitUnspecified62,
    SurgeH265NalUnitUnspecified63,
    SurgeH265NalUnitInvalid
};

@interface SurgeH265NalUnit : NSObject

@property (nonatomic, assign) size_t headerSize;
@property (nonatomic, assign) size_t offset;
@property (nonatomic, assign) SurgeH265NalUnitType type;
@property (nonatomic, assign) size_t length;

- (id)initWithType:(SurgeH265NalUnitType)type atOffset:(size_t)offset withHeaderSize:(size_t)headerSize;

- (void)setBuffer:(const unsigned char *)buffer withLength:(size_t)length;

- (void*)annexBBuffer;

- (void*)bufferWithoutHeader;

- (void)cleanup;

@end
