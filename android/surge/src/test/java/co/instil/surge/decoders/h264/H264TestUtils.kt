/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.h264

import co.instil.surge.decoders.h264.nalu.H264NaluType
import java.lang.System.arraycopy

/**
 * Created by mattmccomb on 14/07/16.
 */
object H264TestUtils {
    @JvmStatic
    fun generateNalUnits(vararg specs: NaluSpec): ByteArray {
        val nalUnitsAsBytes = arrayOfNulls<ByteArray>(specs.size)
        for (i in specs.indices) {
            val spec = specs[i]
            nalUnitsAsBytes[i] = generateNalUnitBytes(spec.headerLength, spec.type, spec.payloadLength)
        }
        return joinNalUnits(*nalUnitsAsBytes)
    }

    fun generateNalUnitBytes(headerLength: Int, type: H264NaluType, payloadLength: Int): ByteArray {
        val payload = ByteArray(headerLength + 1 + payloadLength)
        for (i in 0 until headerLength - 1) {
            payload[i] = 0x00
        }
        payload[headerLength - 1] = 0x01
        payload[headerLength] = type.ordinal.toByte()
        for (i in headerLength + 1 until payloadLength) {
            payload[i] = (Math.abs(Math.random() * 254) + 1).toInt().toByte()
        }
        return payload
    }

    fun joinNalUnits(vararg nalUnits: ByteArray?): ByteArray {
        var totalLength = 0
        for (unit in nalUnits) {
            totalLength += unit?.size ?: 0
        }
        val joinedNalUnits = ByteArray(totalLength)
        var offset = 0
        for (unit in nalUnits) {
            if (unit != null) {
                arraycopy(unit, 0, joinedNalUnits, offset, unit.size)
                offset += unit.size
            }
        }
        return joinedNalUnits
    }

    class NaluSpec(val headerLength: Int, val type: H264NaluType, val payloadLength: Int)
}
