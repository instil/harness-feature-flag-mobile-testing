package co.instil.surge.decoders.h264.nalu;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Parser which extracts {@link NaluSegment} from byte buffer arrays.
 */
public class NaluParser {

    private static final int MINIMUM_NAL_UNIT_LENGTH = 6;

    /**
     * H264 is comprised of NALU segments.
     *
     * XXXX Y ZZZZZZZZ -> XXXX Y ZZZZZZZZ -> XXXX Y ZZZZZZZZ
     *
     * Each segment is comprised of:
     *
     * XXXX   -> Magic byte header (0x00, 0x00, 0x00, 0x01) NOTE: this can be either 3 of 4 bytes
     * Y      -> The Nalu Type
     * ZZZ... -> The Payload
     *
     * Notice there is no nalu length specified. To parse an nalu, you must
     * read until the next magic-byte-sequence AKA the next segment to figure
     * out the full nalu length
     **/
    public List<NaluSegment> parseNaluSegments(ByteBuffer byteBuffer) throws Exception {
        byte[] buffer = new byte[byteBuffer.capacity()];
        byteBuffer.get(buffer);

        List<NaluSegment> segmentList = new ArrayList<>();
        NaluSegment previousSegment = null;

        if (buffer.length < MINIMUM_NAL_UNIT_LENGTH) {
            return segmentList;
        }

        for (int i = 0; i < buffer.length - 10; ++i) {

            int magicByteLength = lengthOfMagicByteSequenceAtPosition(buffer, i);
            boolean isStartOfNalUnit = magicByteLength != -1;
            if (isStartOfNalUnit) {
                int naluType = (buffer[i+magicByteLength] & 0x1F);
                NaluSegment segment = new NaluSegment(naluType, magicByteLength, i);
                segment.setHeaderSize(magicByteLength);
                if (previousSegment != null) {
                    copyBufferIntoSegment(buffer, previousSegment.getOffset(), i - previousSegment.getOffset(), previousSegment);
                }
                segmentList.add(segment);
                previousSegment = segment;
            }
        }

        if (previousSegment != null) {
            copyBufferIntoSegment(buffer, previousSegment.getOffset(), buffer.length - previousSegment.getOffset(), previousSegment);
        }
        return segmentList;
    }

    /**
     * Determines if the next sequence of bytes in the supplied buffer at the specified position
     * represents a magic byte sequence (start of a NAL unit).  The magic byte sequence can be
     * 3 or 4 bytes long (e.g. 0x000001 or 0x00000001).
     * @param buffer byte buffer containing a sequence of NAL units.
     * @param position the position at which the magic byte sequence may start.
     * @return the length of the magic byte sequence at the position (3 or 4); or -1 a magic
     * byte sequence does not start at the supplied position.
     */
    private int lengthOfMagicByteSequenceAtPosition(byte[] buffer, int position) {
        boolean startsWithZeroBytes = buffer[position] == 0x00 && buffer[position+1] == 0x00;
        if (startsWithZeroBytes && buffer[position+2] == 0x01) {
            return 3;
        } else if (startsWithZeroBytes && buffer[position+2] == 0x00 && buffer[position+3] == 0x01) {
            return 4;
        } else {
            return -1;
        }
    }

    private void copyBufferIntoSegment(byte[] buffer, int offset, int length, NaluSegment segment) throws Exception {
        byte[] segmentBuffer = new byte[length];
        System.arraycopy(buffer, offset, segmentBuffer, 0, length);
        segment.setBuffer(segmentBuffer);
    }

}
