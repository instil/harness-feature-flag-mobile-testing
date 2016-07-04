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
