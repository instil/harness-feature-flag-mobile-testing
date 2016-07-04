package co.instil.surge.decoders;

import co.instil.surge.client.SessionDescription;

import java.nio.ByteBuffer;

/**
 *
 */
public class MjpegDecoder implements Decoder {

    @Override
    public void decodeFrameBuffer(SessionDescription sessionDescription,
                                  ByteBuffer frameBuffer,
                                  int width,
                                  int height,
                                  int presentationTime,
                                  int duration) {

    }

    @Override
    public void close() throws InterruptedException {

    }
}
