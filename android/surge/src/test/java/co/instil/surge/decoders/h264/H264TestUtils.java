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

import co.instil.surge.decoders.h264.nalu.NaluType;

/**
 * Created by mattmccomb on 14/07/16.
 */
public class H264TestUtils {

    /**
     *
     */
    public static class NaluSpec {

        private final int headerLength;
        private final NaluType type;
        private final int payloadLength;

        public NaluSpec(int headerLength, NaluType type, int payloadLength) {
            this.headerLength = headerLength;
            this.type = type;
            this.payloadLength = payloadLength;
        }

    }

    public static byte[] generateNalUnits(NaluSpec... specs) {
        byte[][] nalUnitsAsBytes = new byte[specs.length][];
        for (int i = 0; i < specs.length; i++) {
            NaluSpec spec = specs[i];
            nalUnitsAsBytes[i] = generateNalUnitBytes(spec.headerLength, spec.type, spec.payloadLength);
        }
        return joinNalUnits(nalUnitsAsBytes);
    }

    public static byte[] generateNalUnitBytes(int headerLength, NaluType type, int payloadLength) {
        byte[] payload = new byte[headerLength + 1 + payloadLength];
        for (int i = 0; i < headerLength - 1; i++) {
            payload[i] = 0x00;
        }
        payload[headerLength - 1] = 0x01;
        payload[headerLength] = (byte)type.ordinal();
        for (int i = headerLength + 1; i < payloadLength; i++) {
            payload[i] = (byte)(Math.abs(Math.random() * 254) + 1);
        }
        return payload;
    }

    public static byte[] joinNalUnits(byte[]... nalUnits) {
        int totalLength = 0;
        for (byte[] unit : nalUnits) {
            totalLength += unit.length;
        }
        byte[] joinedNalUnits = new byte[totalLength];
        int offset = 0;
        for (byte[] unit : nalUnits) {
            System.arraycopy(unit, 0, joinedNalUnits, offset, unit.length);
            offset += unit.length;
        }
        return joinedNalUnits;
    }

}
