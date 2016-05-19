package co.instil.surge.client;

import java.nio.ByteBuffer;

/**
 *
 */
public interface RtspClientDelegate {

    void clientDidTimeout();

    void clientReceivedFrame(ByteBuffer frameBuffer);

}
