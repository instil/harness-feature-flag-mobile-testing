/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.mpeg.h264

import co.instil.surge.decoders.mpeg.MpegPacket
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluSegment
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluType.CODED_SLICE_IDR
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluType.PPS
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluType.SPS

/**
 * A wrapper around a single H264 payload which provides additional meta-data and
 * utility methods.
 */
class H264Packet(override val segment: H264NaluSegment, override val presentationTime: Long) :
    MpegPacket(segment, presentationTime, 0) {

    override val isKeyFrame = segment.type == CODED_SLICE_IDR

    override val isParameterSet by lazy { segment.type == PPS || segment.type == SPS }

    override fun toString(): String = "H264" + super.toString()
}
