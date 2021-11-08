/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.h264

import co.instil.surge.decoders.h264.nalu.H264NaluSegment
import co.instil.surge.decoders.h264.nalu.H264NaluType
import java.util.Date

/**
 * A wrapper around a single H264 payload which provides additional meta-data and
 * utility methods.
 */
class H264Packet(var segment: H264NaluSegment, var presentationTime: Long) {
    var timestamp = Date()

    /*
     * Determines whether this H264 packet contains a key-frame.
     * @return true if this packet is a key-frame, false otherwise.
     */
    val isKeyFrame: Boolean = segment.type == H264NaluType.CODED_SLICE_IDR

    /*
     * Determines whether this H264 packet contains a SPS (Sequence Parameter Set).
     * @return true if this packet contains a SPS, false otherwise.
     */
    val isSequenceParameterSet: Boolean = segment.type == H264NaluType.SPS

    /*
     * Determines whether this H264 packet contains a PPS (Picture Parameter Set).
     * @return true if this packet contains a PPS, false otherwise.
     */
    val isPictureParameterSet: Boolean = segment.type == H264NaluType.PPS

    override fun toString(): String = String.format(
        "H264 packet: %s frame of size %d bytes received at %s",
        if (isKeyFrame) "key" else "non-key",
        segment.payloadLength,
        timestamp
    )
}
