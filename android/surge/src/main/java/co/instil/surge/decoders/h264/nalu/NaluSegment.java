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
public class NaluSegment {

    private NaluType type;
    private byte[] payload;

    public NaluSegment(NaluType type, byte[] bytes) {
        this.type = type;
        payload = new byte[bytes.length + 4];
        for (int i = 0; i < 3; i++) {
            payload[i] = 0x00;
        }
        payload[3] = 0x01;
        System.arraycopy(bytes, 0, this.payload, 4, bytes.length);
    }

    public byte[] getPayload() {
        return payload;
    }

    public int getPayloadLength() {
        return payload.length;
    }

    public NaluType getType() {
        return type;
    }

    @Override
    public String toString() {
        return String.format("\n{\n\ttype: \"%s\", \n\tpayload: \"%s\"}", type, payload);
    }
}
