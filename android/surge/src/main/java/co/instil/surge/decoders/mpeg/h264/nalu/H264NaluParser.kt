/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.mpeg.h264.nalu

import co.instil.surge.decoders.mpeg.NaluParser
import co.instil.surge.decoders.mpeg.NaluSegment
import co.instil.surge.decoders.mpeg.NaluType
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluType.Companion.getTypeFromCode
import kotlin.experimental.and

class H264NaluParser : NaluParser() {
    override fun naluSegment(naluType: NaluType, payload: ByteArray): NaluSegment =
        H264NaluSegment(naluType as H264NaluType, payload)

    override fun getNaluType(stream: ByteArray, header: NaluMagicByteHeader): NaluType =
        getTypeFromCode((stream[header.position + header.length] and 0x1F).toInt())
}
