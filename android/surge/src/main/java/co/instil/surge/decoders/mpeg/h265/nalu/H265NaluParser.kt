/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg.h265.nalu

import co.instil.surge.decoders.mpeg.NaluParser
import co.instil.surge.decoders.mpeg.NaluSegment
import co.instil.surge.decoders.mpeg.NaluType
import kotlin.experimental.and

class H265NaluParser : NaluParser() {
    override fun naluSegment(naluType: NaluType, payload: ByteArray): NaluSegment =
        H265NaluSegment(naluType as H265NaluType, payload)

    override fun getNaluType(stream: ByteArray, header: NaluMagicByteHeader): NaluType =
        H265NaluType.values()[((stream[header.position + header.length] and 0x7E)).toInt().shr(1)]
}
