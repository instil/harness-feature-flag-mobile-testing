package co.instil.surge.decoders.h265

import co.instil.surge.decoders.h265.nalu.H265NaluType
import java.lang.Math.random
import kotlin.math.abs

class H265TestUtils {
    companion object {
        fun generateH265NalUnit(headerLength: Int, type: H265NaluType, payloadLength: Int): ByteArray {
            val payload = ByteArray(headerLength + 1 + payloadLength)
            for (i in 0 until headerLength - 1) {
                payload[i] = 0x00
            }
            payload[headerLength - 1] = 0x01
            payload[headerLength] = type.ordinal.shl(1).toByte()
            for (i in headerLength + 1 until payload.size) {
                payload[i] = (abs(random() * 254) + 1).toInt().toByte()
            }
            return payload
        }

        fun joinH265NalUnits(vararg nalUnits: ByteArray): ByteArray {
            var totalLength = 0
            for (unit in nalUnits) {
                totalLength += unit.size
            }
            val joinedNalUnits = ByteArray(totalLength)
            var offset = 0
            for (unit in nalUnits) {
                System.arraycopy(unit, 0, joinedNalUnits, offset, unit.size)
                offset += unit.size
            }
            return joinedNalUnits
        }
    }
}