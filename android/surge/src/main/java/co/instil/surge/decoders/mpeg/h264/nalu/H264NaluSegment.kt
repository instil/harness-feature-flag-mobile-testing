/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.mpeg.h264.nalu

import co.instil.surge.decoders.mpeg.NaluSegment

class H264NaluSegment(override val type: H264NaluType, bytes: ByteArray) : NaluSegment(type, bytes) {
    init {
        payload = ByteArray(bytes.size + 4)

        for (i in 0..2) {
            payload[i] = 0x00
        }
        payload[3] = 0x01
        System.arraycopy(bytes, 0, payload, 4, bytes.size)
    }

    override fun isParameterSet() = false
}
