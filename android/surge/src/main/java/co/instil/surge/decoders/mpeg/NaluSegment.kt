/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg

abstract class NaluSegment(open val type: NaluType, bytes: ByteArray) {
    protected var magicHeaderLength: Int = 0
    lateinit var payload: ByteArray

    val payloadLength: Int
        get() = payload.size

    protected abstract fun isParameterSet(): Boolean

    override fun toString() = "\n{\n\ttype: \"$type\", \n\tpayload length: \"${payloadLength}\"}"
}
