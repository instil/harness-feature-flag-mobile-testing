package co.instil.surge.decoders.h265.nalu

import java.nio.ByteBuffer
import kotlin.experimental.and

class H265NaluParser {
    fun parseH265NaluSegments(byteBuffer: ByteBuffer): List<H265NaluSegment> {
        val buffer = byteBuffer.copyToByteArray()
        if (buffer.size <= MINIMUM_NAL_UNIT_LENGTH) {
            return ArrayList()
        }
        val headers = findMagicByteHeadersInStream(buffer)
        return extractNalUnitsInStream(headers, buffer)
    }

    private fun findMagicByteHeadersInStream(stream: ByteArray): List<H265NaluMagicByteHeader> {
        val headers: MutableList<H265NaluMagicByteHeader> = ArrayList()
        var i = 0
        while (i < stream.size - MINIMUM_MAGIC_HEADER_LENGTH) {
            val magicByteLength: Int = lengthOfMagicByteSequenceAtPosition(stream, i)
            val isStartOfNalUnit = magicByteLength != -1
            if (isStartOfNalUnit) {
                headers.add(H265NaluMagicByteHeader(i, magicByteLength))
                i += magicByteLength
            }
            ++i
        }
        return headers
    }

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

    private fun extractNalUnitsInStream(headers: List<H265NaluMagicByteHeader>, stream: ByteArray): List<H265NaluSegment> {
        return headers.mapIndexed { index, header ->
            val naluType = getH265NaluType(stream, header)
            val payload = getPayload(index, headers, stream, header)
            H265NaluSegment(naluType, payload)
        }.toList()
    }

    private fun getH265NaluType(stream: ByteArray, header: H265NaluMagicByteHeader) =
        H265NaluType.values()[((stream[header.position + header.length] and 0x7E)).toInt().shr(1)]

    private fun getPayload(index: Int, headers: List<H265NaluMagicByteHeader>, stream: ByteArray, header: H265NaluMagicByteHeader): ByteArray {
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

    private class H265NaluMagicByteHeader(val position: Int, val length: Int)
    companion object {
        private const val MINIMUM_MAGIC_HEADER_LENGTH = 3
        private const val MAXIMUM_MAGIC_HEADER_LENGTH = 4
        private const val MINIMUM_NAL_UNIT_LENGTH = 6
        private const val ZERO_BYTE: Byte = 0x00
        private const val MAGIC_BYTE: Byte = 0x01
    }
}
