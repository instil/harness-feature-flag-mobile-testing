package co.instil.surge.decoders;

import java.nio.ByteBuffer;

/**
 *
 */
public interface Decoder {

    void decodeFrameBuffer(ByteBuffer frameBuffer, int duration, int presentationTime);

}
