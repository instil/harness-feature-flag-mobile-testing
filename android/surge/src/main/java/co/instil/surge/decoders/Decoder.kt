/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders

import co.instil.surge.client.SessionDescription
import java.nio.ByteBuffer
import kotlin.Throws

interface Decoder {
    fun decodeFrameBuffer(
        sessionDescription: SessionDescription,
        frameBuffer: ByteBuffer,
        width: Int,
        height: Int,
        presentationTime: Int,
        duration: Int
    )

    @Throws(InterruptedException::class)
    fun close()
}
