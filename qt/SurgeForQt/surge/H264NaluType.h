#ifndef H264NALUTYPE_H
#define H264NALUTYPE_H

#include <stdint.h>

enum class H264NaluType : uint8_t {
    UNSPECIFIED, // 0: Unspecified (non-VCL)
    CODED_SILCE_NON_IDR, // 1: Coded slice of a non-IDR picture (VCL)
    CODED_SLICE_DATA_PART_A, // 2: Coded slice data partition A (VCL)
    CODED_SLICE_DATA_PART_B, // 3: Coded slice data partition B (VCL)
    CODED_SLICE_DATA_PART_C, // 4: Coded slice data partition C (VCL)
    CODED_SLICE_IDR, // 5: Coded slice of an IDR picture (VCL)
    SUPP_ENHANCEMENT, // 6: Supplemental enhancement information (SEI) (non-VCL)
    SPS, // 7: Sequence parameter set (non-VCL)
    PPS, // 8: Picture parameter set (non-VCL)
    ACC_DELIM, // 9: Access unit delimiter (non-VCL)
    END_SEQ, // 10: End of sequence (non-VCL)
    END_STREAM, // 11: End of stream (non-VCL)
    FILLER, // 12: Filler data (non-VCL)
    SPS_EXT, // 13: Sequence parameter set extension (non-VCL)
    PREFIX, // 14: Prefix NAL unit (non-VCL)
    SUBSET_SPS, // 15: Subset sequence parameter set (non-VCL)
    RESERVED_16, // 16: Reserved (non-VCL)
    RESERVED_17, // 17: Reserved (non-VCL)
    RESERVED_18, // 18: Reserved (non-VCL)
    CODED_SLICE_AUX, // 19: Coded slice of an auxiliary coded picture without partitioning (non-VCL)
    CODED_SLICE_EXT_20, // 20: Coded slice extension (non-VCL)
    CODED_SLICE_EXT_21, // 21: Coded slice extension for depth view components (non-VCL)
    RESERVED_22, // 22: Reserved (non-VCL)
    RESERVED_23, // 23: Reserved (non-VCL)
    STAP_A, // 24: STAP-A Single-time aggregation packet (non-VCL)
    STAP_B, // 25: STAP-B Single-time aggregation packet (non-VCL)
    MTAP16, // 26: MTAP16 Multi-time aggregation packet (non-VCL)
    MTAP24, // 27: MTAP24 Multi-time aggregation packet (non-VCL)
    FU_A, // 28: FU-A Fragmentation unit (non-VCL)
    FU_B, // 29: FU-B Fragmentation unit (non-VCL)
    UNSPECIFIED_30, // 30: Unspecified (non-VCL)
    UNSPECIFIED_31 // 31: Unspecified (non-VCL)
};

#endif // H264NALUTYPE_H
