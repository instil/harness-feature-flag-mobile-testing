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

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Parser which extracts {@link NaluSegment} from byte buffer arrays.
 *
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
 */
public class NaluParser {

    private static final int MINIMUM_MAGIC_HEADER_LENGTH = 3;
    private static final int MAXIMUM_MAGIC_HEADER_LENGTH = 4;
    private static final int MINIMUM_NAL_UNIT_LENGTH = 6;

    /*
     * Parse all of the NAL units contained within the given buffer.
      * @param byteBuffer the buffer containing the nal units to be parsed.
      * @returns a list of parsed NAL units or an empty list if the supplied buffer did not
      * contain any valid units.
     */
    public List<NaluSegment> parseNaluSegments(ByteBuffer byteBuffer) throws Exception {
        byte[] buffer = new byte[byteBuffer.capacity()];
        byteBuffer.get(buffer);
        if (buffer.length <= MINIMUM_NAL_UNIT_LENGTH) {
            return new ArrayList<>();
        }
        List<NaluMagicByteHeader> headers = findMagicByteHeadersInStream(buffer);
        return extractNalUnitsInStream(headers, buffer);
    }

    /*
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
            return MINIMUM_MAGIC_HEADER_LENGTH;
        } else if (startsWithZeroBytes && buffer[position+2] == 0x00 && buffer[position+3] == 0x01) {
            return MAXIMUM_MAGIC_HEADER_LENGTH;
        } else {
            return -1;
        }
    }

    /*
     * Iterates over the supplied stream to find the starting positions of each NAL unit within
     * said stream (as determined by the presence of a magic byte header 0x000001 or 0x00000001).
     */
    private List<NaluMagicByteHeader> findMagicByteHeadersInStream(byte[] stream) {
        List<NaluMagicByteHeader> headers = new ArrayList<>();
        for (int i = 0; i < stream.length - MINIMUM_MAGIC_HEADER_LENGTH; ++i) {
            int magicByteLength = lengthOfMagicByteSequenceAtPosition(stream, i);
            boolean isStartOfNalUnit = magicByteLength != -1;
            if (isStartOfNalUnit) {
                headers.add(new NaluMagicByteHeader(i, magicByteLength));
                i += magicByteLength;
            }
        }
        return headers;
    }

    /*
     * Extrapolates the actual NAL units from a buffer given the positions of the headers within
     * that buffer.
     */
    private List<NaluSegment> extractNalUnitsInStream(List<NaluMagicByteHeader> headers, byte[] stream) {
        List<NaluSegment> segmentList = new ArrayList<>();
        for (int i = 0; i < headers.size(); i++) {
            NaluMagicByteHeader header = headers.get(i);
            boolean isLastHeader = i == headers.size() - 1;
            NaluType naluType = NaluType.getTypeFromCode(stream[header.position + header.length] & 0x1F);
            byte[] payload = Arrays.copyOfRange(stream, header.position + header.length, stream.length);
            if (!isLastHeader) {
                NaluMagicByteHeader nextHeader = headers.get(i + 1);
                payload = Arrays.copyOfRange(stream, header.position + header.length, nextHeader.position);
            }
            segmentList.add(new NaluSegment(naluType, payload));
        }
        return segmentList;
    }

    private class NaluMagicByteHeader {

        private final int position;
        private final int length;

        public NaluMagicByteHeader(int position, int length) {
            this.position = position;
            this.length = length;
        }

    }

}
