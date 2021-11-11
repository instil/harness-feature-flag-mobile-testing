/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg.h265.nalu

import co.instil.surge.decoders.mpeg.NaluSegment

class H265NaluSegment(override val type: H265NaluType, bytes: ByteArray) : NaluSegment(type, bytes) {
    init {
        magicHeaderLength = if (isParameterSet()) 4 else 3
        payload = ByteArray(bytes.size + magicHeaderLength)

        for (i in 0..(magicHeaderLength - 2)) {
            payload[i] = 0x00
        }
        payload[magicHeaderLength - 1] = 0x01
        System.arraycopy(bytes, 0, payload, magicHeaderLength, bytes.size)
    }

    override fun isParameterSet() =
        type == H265NaluType.VPS || type == H265NaluType.PPS || type == H265NaluType.SPS
}
