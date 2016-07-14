/*
 * Copyright (c) 2016 Instil Software.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

package co.instil.surge.decoders.h264.nalu;

/**
 *
 */
public enum NaluType {
    UNSPECIFIED("0: Unspecified (non-VCL)"),
    CODED_SILCE_NON_IDR("1: Coded slice of a non-IDR picture (VCL)"),
    CODED_SLICE_DATA_PART_A("2: Coded slice data partition A (VCL)"),
    CODED_SLICE_DATA_PART_B("3: Coded slice data partition B (VCL)"),
    CODED_SLICE_DATA_PART_C("4: Coded slice data partition C (VCL)"),
    CODED_SLICE_IDR("5: Coded slice of an IDR picture (VCL)"),
    SUPP_ENHANCEMENT("6: Supplemental enhancement information (SEI) (non-VCL)"),
    SPS("7: Sequence parameter set (non-VCL)"),
    PPS("8: Picture parameter set (non-VCL)"),
    ACC_DELIM("9: Access unit delimiter (non-VCL)"),
    END_SEQ("10: End of sequence (non-VCL)"),
    END_STREAM("11: End of stream (non-VCL)"),
    FILLER("12: Filler data (non-VCL)"),
    SPS_EXT("13: Sequence parameter set extension (non-VCL)"),
    PREFIX("14: Prefix NAL unit (non-VCL)"),
    SUBSET_SPS("15: Subset sequence parameter set (non-VCL)"),
    RESERVED_16("16: Reserved (non-VCL)"),
    RESERVED_17("17: Reserved (non-VCL)"),
    RESERVED_18("18: Reserved (non-VCL)"),
    CODED_SLICE_AUX("19: Coded slice of an auxiliary coded picture without partitioning (non-VCL)"),
    CODED_SLICE_EXT_20("20: Coded slice extension (non-VCL)"),
    CODED_SLICE_EXT_21("21: Coded slice extension for depth view components (non-VCL)"),
    RESERVED_22("22: Reserved (non-VCL)"),
    RESERVED_23("23: Reserved (non-VCL)"),
    STAP_A("24: STAP-A Single-time aggregation packet (non-VCL)"),
    STAP_B("25: STAP-B Single-time aggregation packet (non-VCL)"),
    MTAP16("26: MTAP16 Multi-time aggregation packet (non-VCL)"),
    MTAP24("27: MTAP24 Multi-time aggregation packet (non-VCL)"),
    FU_A("28: FU-A Fragmentation unit (non-VCL)"),
    FU_B("29: FU-B Fragmentation unit (non-VCL)"),
    UNSPECIFIED_30("30: Unspecified (non-VCL)"),
    UNSPECIFIED_31("31: Unspecified (non-VCL)");

    private String description;
    private static final String TAG = "NaluType";

    NaluType(String description) {
        this.description = description;
    }

    public String toString() {
        return description;
    }

    public String getDescription() {
        return description;
    }

    public static NaluType getTypeFromCode(int rawType) {
        NaluType type = NaluType.UNSPECIFIED;
        switch(rawType) {
            case 1:
                type = NaluType.CODED_SILCE_NON_IDR;
                break;

            case 5:
                type = NaluType.CODED_SLICE_IDR;
                break;

            case 7:
                type = NaluType.SPS;
                break;

            case 8:
                type = NaluType.PPS;
                break;

            default:
                break;
        }
        return type;
    }
}
