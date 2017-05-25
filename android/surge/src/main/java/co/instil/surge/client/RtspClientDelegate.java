/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.client;

import java.nio.ByteBuffer;

/**
 *
 */
public interface RtspClientDelegate {

    void clientDidTimeout();

    void clientReceivedFrame(ByteBuffer frameBuffer,
                             int width,
                             int height,
                             int presentationTime,
                             int duration);

    void clientReceivedExtendedHeader(ByteBuffer buffer,
                                      int length);

}
