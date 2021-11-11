/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg

import java.nio.ByteBuffer

/**
 * Parser which extracts [NaluSegment] from byte buffer arrays.
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
abstract class NaluParser {
    /*
     * Parse all of the NAL units contained within the given buffer.
     * @param byteBuffer the buffer containing the nal units to be parsed.
     * @returns a list of parsed NAL units or an empty list if the supplied buffer did not
     * contain any valid units.
     */
    fun parseNaluSegments(byteBuffer: ByteBuffer): List<NaluSegment> {
        val buffer = byteBuffer.copyToByteArray()
        if (buffer.size <= MINIMUM_NAL_UNIT_LENGTH) {
            return ArrayList()
        }
        val headers = findMagicByteHeadersInStream(buffer)
        return extractNalUnitsInStream(headers, buffer)
    }

    /*
     * Iterates over the supplied stream to find the starting positions of each NAL unit within
     * said stream (as determined by the presence of a magic byte header 0x000001 or 0x00000001).
     */
    private fun findMagicByteHeadersInStream(stream: ByteArray): List<NaluMagicByteHeader> {
        val headers: MutableList<NaluMagicByteHeader> = ArrayList()
        var i = 0
        while (i < stream.size - MINIMUM_MAGIC_HEADER_LENGTH) {
            val magicByteLength: Int = lengthOfMagicByteSequenceAtPosition(stream, i)
            val isStartOfNalUnit = magicByteLength != -1
            if (isStartOfNalUnit) {
                headers.add(NaluMagicByteHeader(i, magicByteLength))
                i += magicByteLength
            }
            ++i
        }
        return headers
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
    private fun lengthOfMagicByteSequenceAtPosition(buffer: ByteArray, position: Int): Int {
        val startsWithZeroBytes = buffer[position] == ZERO_BYTE && buffer[position + 1] == ZERO_BYTE
        return if (startsWithZeroBytes && buffer[position + 2] == MAGIC_BYTE) {
            MINIMUM_MAGIC_HEADER_LENGTH
        } else if (startsWithZeroBytes && buffer[position + 2] == ZERO_BYTE && buffer[position + 3] == MAGIC_BYTE) {
            MAXIMUM_MAGIC_HEADER_LENGTH
        } else {
            -1
        }
    }

    /*
     * Extrapolate the actual NAL units from a buffer given the positions of the headers within
     * that buffer.
     */
    private fun extractNalUnitsInStream(headers: List<NaluMagicByteHeader>, stream: ByteArray): List<NaluSegment> {
        return headers.mapIndexed { index, header ->
            val naluType = getNaluType(stream, header)
            val payload = getPayload(index, headers, stream, header)
            naluSegment(naluType, payload)
        }.toList()
    }

    protected abstract fun naluSegment(naluType: NaluType, payload: ByteArray): NaluSegment

    protected abstract fun getNaluType(stream: ByteArray, header: NaluMagicByteHeader): NaluType

    private fun getPayload(index: Int, headers: List<NaluMagicByteHeader>, stream: ByteArray, header: NaluMagicByteHeader): ByteArray {
        val isLastHeader = index == headers.size - 1
        return if (isLastHeader) {
            stream.copyOfRange(header.position + header.length, stream.size)
        } else {
            val nextHeader = headers[index + 1]
            stream.copyOfRange(header.position + header.length, nextHeader.position)
        }
    }

    private fun ByteBuffer.copyToByteArray(): ByteArray {
        val buffer = ByteArray(this.capacity())
        this.get(buffer)
        return buffer
    }

    protected inner class NaluMagicByteHeader(val position: Int, val length: Int)

    companion object {
        internal const val MINIMUM_MAGIC_HEADER_LENGTH = 3
        internal const val MAXIMUM_MAGIC_HEADER_LENGTH = 4
        internal const val MINIMUM_NAL_UNIT_LENGTH = 6
        internal const val ZERO_BYTE: Byte = 0x00
        internal const val MAGIC_BYTE: Byte = 0x01
    }
}
