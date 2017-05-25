/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
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
        return String.format("\n{\n\ttype: \"%s\", \n\tpayload length: \"%s\"}", type, payload.length);
    }
}
