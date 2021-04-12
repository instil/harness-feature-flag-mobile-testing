package co.instil.surge.decoders.h265

import co.instil.surge.decoders.h265.nalu.H265NaluSegment
import co.instil.surge.decoders.h265.nalu.H265NaluType.CODED_SLICE_CRA
import co.instil.surge.decoders.h265.nalu.H265NaluType.CODED_SLICE_IDR
import co.instil.surge.decoders.h265.nalu.H265NaluType.CODED_SLICE_IDR_N_LP
import co.instil.surge.decoders.h265.nalu.H265NaluType.PPS
import co.instil.surge.decoders.h265.nalu.H265NaluType.SPS
import co.instil.surge.decoders.h265.nalu.H265NaluType.VPS
import java.util.Date

class H265Packet(val naluSegment: H265NaluSegment, val presentationTime: Long, val flags: Int) {
    val isKeyFrame by lazy {
        naluSegment.type == CODED_SLICE_IDR ||
            naluSegment.type == CODED_SLICE_IDR_N_LP ||
            naluSegment.type == CODED_SLICE_CRA
    }

    val isVideoParameterSet
        get() = naluSegment.type == VPS

    val isSequenceParameterSet
        get() = naluSegment.type == SPS

    val isPictureParameterSet
        get() = naluSegment.type == PPS

    constructor(naluSegment: H265NaluSegment, presentationTime: Long) : this(naluSegment, presentationTime, 0)

    override fun toString(): String {
        return String.format(
            "H265 packet: %s frame of size %d bytes received at %s",
            if (isKeyFrame) "key" else "non-key",
            naluSegment.payloadLength,
            Date()
        )
    }
}
