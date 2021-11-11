/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg

import java.util.Date

abstract class MpegPacket(
    open val segment: NaluSegment,
    open val presentationTime: Long,
    open val flags: Int
) {
    private var timestamp = Date()

    abstract val isKeyFrame: Boolean

    abstract val isParameterSet: Boolean

    override fun toString(): String = " packet: " +
        "${if (isKeyFrame) "key" else "non-key"} frame " +
        "of size ${segment.payloadLength} bytes " +
        "received at $timestamp"
}
