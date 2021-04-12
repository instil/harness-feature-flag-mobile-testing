/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.h264;

import co.instil.surge.decoders.h264.nalu.NaluSegment;
import co.instil.surge.decoders.h264.nalu.NaluType;

import java.util.Date;
import java.util.Locale;

/**
 * A wrapper around a single H264 payload which provides additional meta-data and
 * utility methods.
 */
public class H264Packet {

    public long presentationTime;
    public NaluSegment segment;
    public Date timestamp;

    /*
     * Constructs a H264Packet instance with the supplied NALU segment and presentation timestamp.
     * @param segment the NALU segment for this packet
     * @param presentationTime the presentation time associated with the NALU segment
     */
    public H264Packet(NaluSegment segment, long presentationTime) {
        this.presentationTime = presentationTime;
        this.segment = segment;
        this.timestamp = new Date();
    }

    /*
     * Determines whether this H264 packet contains a key-frame.
     * @return true if this packet is a key-frame, false otherwise.
     */
    public boolean isKeyFrame() {
        return segment.getType() == NaluType.CODED_SLICE_IDR;
    }

    /*
     * Determines whether this H264 packet contains a SPS (Sequence Parameter Set).
     * @return true if this packet contains a SPS, false otherwise.
     */
    public boolean isSequenceParameterSet() {
        return segment.getType() == NaluType.SPS;
    }

    /*
     * Determines whether this H264 packet contains a PPS (Picture Parameter Set).
     * @return true if this packet contains a PPS, false otherwise.
     */
    public boolean isPictureParameterSet() {
        return segment.getType() == NaluType.PPS;
    }

    public String toString() {
        return String.format(Locale.ENGLISH,
                "H264 packet: %s frame of size %d bytes received at %s",
                isKeyFrame() ? "key" : "non-key",
                segment.getPayloadLength(),
                timestamp);
    }

}
