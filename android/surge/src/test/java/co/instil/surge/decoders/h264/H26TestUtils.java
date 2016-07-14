package co.instil.surge.decoders.h264;

import co.instil.surge.decoders.h264.nalu.NaluType;

/**
 * Created by mattmccomb on 14/07/16.
 */
public class H26TestUtils {

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
