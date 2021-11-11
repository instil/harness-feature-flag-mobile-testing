/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg.h265

import co.instil.surge.decoders.mpeg.MpegPacket
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluSegment
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.CODED_SLICE_CRA
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.CODED_SLICE_IDR
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.CODED_SLICE_IDR_N_LP
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.PPS
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.SPS
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.VPS

class H265Packet(override val segment: H265NaluSegment, override val presentationTime: Long, override val flags: Int) :
    MpegPacket(segment, presentationTime, flags) {

    override val isKeyFrame by lazy {
        segment.type == CODED_SLICE_IDR ||
            segment.type == CODED_SLICE_IDR_N_LP ||
            segment.type == CODED_SLICE_CRA
    }

    override val isParameterSet by lazy { segment.type == PPS || segment.type == SPS || segment.type == VPS }

    constructor(naluSegment: H265NaluSegment, presentationTime: Long) : this(naluSegment, presentationTime, 0)

    override fun toString(): String = "H265" + super.toString()
}
