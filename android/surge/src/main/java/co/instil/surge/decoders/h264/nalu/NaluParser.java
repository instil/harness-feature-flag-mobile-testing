package co.instil.surge.decoders.h264.nalu;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class NaluParser {
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
    public static List<NaluSegment> parseNaluSegments(ByteBuffer byteBuffer) throws Exception {
        byte[] buffer = new byte[byteBuffer.capacity()];
        byteBuffer.get(buffer);

        List<NaluSegment> segmentList = new ArrayList<>();
        if (buffer.length < 6) {
            return segmentList;
        }

        int lastStartingOffset = -1;
        for (int i = 0; i < buffer.length - 10; ++i) {
            if (buffer[i] == 0x00 && buffer[i+1] == 0x00 && buffer[i+2] == 0x01) {
                int naluType = (buffer[i+3] & 0x1F);
                NaluSegment segment = new NaluSegment(naluType, 3, i);

                if (i > 0 && buffer[i-1] == 0x00) {
                    // This is actually a 4 byte segment
                    int currentSegmentOffset = segment.getOffset();
                    segment.setHeaderSize(4);
                    segment.setOffset(currentSegmentOffset - 1);
                }

                if (lastStartingOffset > -1) {
                    NaluSegment lastSegment = segmentList.get(segmentList.size() - 1);

                    int bufferLength = segment.getOffset() - lastSegment.getOffset();
                    byte[] segmentBuffer = new byte[bufferLength];

                    System.arraycopy(buffer, lastSegment.getOffset(), segmentBuffer, 0, bufferLength);
                    lastSegment.setBuffer(segmentBuffer);

                    segmentList.remove(segmentList.size() - 1);
                    segmentList.add(lastSegment);
                }

                segmentList.add(segment);
                lastStartingOffset = segment.getOffset();
            }
        }

        if (segmentList.size() > 0) {
            NaluSegment lastSegment = segmentList.get(segmentList.size() - 1);

            int bufferLength = buffer.length - lastSegment.getOffset();
            byte[] segmentBuffer = new byte[bufferLength];
            System.arraycopy(buffer, lastSegment.getOffset(), segmentBuffer, 0, bufferLength);
            lastSegment.setBuffer(segmentBuffer);

            segmentList.remove(segmentList.size() - 1);
            segmentList.add(lastSegment);
        }
        return segmentList;
    }

}
