/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.h264.nalu

class H264NaluSegment(val type: H264NaluType, bytes: ByteArray) {
    val payload: ByteArray = ByteArray(bytes.size + 4)

    val payloadLength: Int = payload.size

    init {
        for (i in 0..2) {
            payload[i] = 0x00
        }
        payload[3] = 0x01
        System.arraycopy(bytes, 0, payload, 4, bytes.size)
    }

    override fun toString() = "\n{\n\ttype: \"$type\", \n\tpayload length: \"${payload.size}\"}"
}
