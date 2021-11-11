/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg.h265.nalu

import co.instil.surge.decoders.mpeg.NaluType

enum class H265NaluType(private val description: String) : NaluType {
    CODED_SLICE_TRAIL_N("0: Coded slice segment of a non-TSA, non-STSA trailing picture (VCL)"),
    CODED_SLICE_TRAIL_R("1: Coded slice segment of a non-TSA, non-STSA trailing picture (VCL)"),
    CODED_SLICE_TSA_N("2: Coded slice segment of a Temporal Sub-layer Access (TSA) picture (VCL)"),
    CODED_SLICE_TLA("3: Coded slice segment of a Temporal Sub-layer Access (TSA) picture (VCL)"), // Current name in the spec: TSA_R
    CODED_SLICE_STSA_N("4: Coded slice segment of an Step-wise Temporal Sub-layer Access (STSA) picture (VCL)"),
    CODED_SLICE_STSA_R("5: Coded slice segment of an Step-wise Temporal Sub-layer Access (STSA) picture (VCL)"),
    CODED_SLICE_RADL_N("6: Coded slice segment of a Random Access Decodable Leading (RADL) picture (VCL)"),
    CODED_SLICE_DLP("7: Coded slice segment of a Random Access Decodable Leading (RADL) picture (VCL)"), // Current name in the spec: RADL_R
    CODED_SLICE_RASL_N("8: Coded slice segment of a Random Access Skipped Leading (RASL) picture (VCL)"),
    CODED_SLICE_TFD("9: Coded slice segment of a Random Access Skipped Leading (RASL) picture (VCL)"), // Current name in the spec: RASL_R
    RESERVED_10("10: Reserved non-IRAP SLNR VCL NAL unit types"),
    RESERVED_11("11: Reserved non-IRAP sub-layer reference VCL NAL unit types"),
    RESERVED_12("12: Reserved non-IRAP SLNR VCL NAL unit types"),
    RESERVED_13("13: Reserved non-IRAP sub-layer reference VCL NAL unit types"),
    RESERVED_14("14: Reserved non-IRAP SLNR VCL NAL unit types"),
    RESERVED_15("15: Reserved non-IRAP sub-layer reference VCL NAL unit types"),
    CODED_SLICE_BLA("16: Coded slice segment of a Broken Link Access (BLA) picture (VCL)"), // Current name in the spec: BLA_W_LP
    CODED_SLICE_BLANT("17: Coded slice segment of a Broken Link Access (BLA) picture (VCL)"), // Current name in the spec: BLA_W_DLP
    CODED_SLICE_BLA_N_LP("18: Coded slice segment of a Broken Link Access (BLA) picture (VCL)"),
    CODED_SLICE_IDR("19: Coded slice segment of an Instantaneous Decoding Refresh (IDR) picture (VCL)"), // Current name in the spec: IDR_W_DLP
    CODED_SLICE_IDR_N_LP("20: Coded slice segment of an Instantaneous Decoding Refresh (IDR) picture (VCL)"),
    CODED_SLICE_CRA("21: Coded slice segment of a Clean Random Access (CRA) picture (VCL)"),
    RESERVED_22("22: Reserved Intra Random Access Point (IRAP) VCL NAL unit types"),
    RESERVED_23("23: Reserved Intra Random Access Point (IRAP) VCL NAL unit types"),
    RESERVED_24("24: Reserved non-IRAP VCL NAL unit types"),
    RESERVED_25("25: Reserved non-IRAP VCL NAL unit types"),
    RESERVED_26("26: Reserved non-IRAP VCL NAL unit types"),
    RESERVED_27("27: Reserved non-IRAP VCL NAL unit types"),
    RESERVED_28("28: Reserved non-IRAP VCL NAL unit types"),
    RESERVED_29("29: Reserved non-IRAP VCL NAL unit types"),
    RESERVED_30("30: Reserved non-IRAP VCL NAL unit types"),
    RESERVED_31("31: Reserved non-IRAP VCL NAL unit types"),
    VPS("32: Video parameter set (non-VCL)"),
    SPS("33: Sequence parameter set (non-VCL)"),
    PPS("34: Picture parameter set (non-VCL)"),
    ACCESS_UNIT_DELIMITER("35: Access unit delimiter (non-VCL)"),
    EOS("36: End of sequence (non-VCL)"),
    EOB("37: End of bitstream (non-VCL)"),
    FILLER_DATA("38: Filler data (non-VCL)"),
    SEI("39: Supplemental enhancement information prefix (non-VCL)"),
    SEI_SUFFIX("40: Supplemental enhancement information suffix (non-VCL)"),
    RESERVED_41("41: Reserved (non-VCL)"),
    RESERVED_42("42: Reserved (non-VCL)"),
    RESERVED_43("43: Reserved (non-VCL)"),
    RESERVED_44("44: Reserved (non-VCL)"),
    RESERVED_45("45: Reserved (non-VCL)"),
    RESERVED_46("46: Reserved (non-VCL)"),
    RESERVED_47("47: Reserved (non-VCL)"),
    UNSPECIFIED_48("48: Unspecified (non-VCL)"),
    UNSPECIFIED_49("48: Unspecified (non-VCL)"),
    UNSPECIFIED_50("50: Unspecified (non-VCL)"),
    UNSPECIFIED_51("51: Unspecified (non-VCL)"),
    UNSPECIFIED_52("52: Unspecified (non-VCL)"),
    UNSPECIFIED_53("53: Unspecified (non-VCL)"),
    UNSPECIFIED_54("54: Unspecified (non-VCL)"),
    UNSPECIFIED_55("55: Unspecified (non-VCL)"),
    UNSPECIFIED_56("56: Unspecified (non-VCL)"),
    UNSPECIFIED_57("57: Unspecified (non-VCL)"),
    UNSPECIFIED_58("58: Unspecified (non-VCL)"),
    UNSPECIFIED_59("59: Unspecified (non-VCL)"),
    UNSPECIFIED_60("60: Unspecified (non-VCL)"),
    UNSPECIFIED_61("61: Unspecified (non-VCL)"),
    UNSPECIFIED_62("62: Unspecified (non-VCL)"),
    UNSPECIFIED_63("63: Unspecified (non-VCL)"),
    INVALID("Invalid");

    override fun toString(): String = description
}
