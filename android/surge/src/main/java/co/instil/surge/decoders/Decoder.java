/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders;

import co.instil.surge.client.SessionDescription;

import java.nio.ByteBuffer;

/**
 *
 */
public interface Decoder {

    void decodeFrameBuffer(SessionDescription sessionDescription,
                           ByteBuffer frameBuffer,
                           int width,
                           int height,
                           int presentationTime,
                           int duration);

    void close() throws InterruptedException;
}
