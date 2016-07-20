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

package co.instil.surge.decoders.h264;

import co.instil.surge.decoders.h264.nalu.NaluSegment;
import co.instil.surge.decoders.h264.nalu.NaluType;

import java.util.Date;


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
        return String.format("H264 packet: %s frame of size %d bytes received at %s",
                isKeyFrame() ? "key" : "non-key",
                segment.getPayloadLength(),
                timestamp);
    }

}
