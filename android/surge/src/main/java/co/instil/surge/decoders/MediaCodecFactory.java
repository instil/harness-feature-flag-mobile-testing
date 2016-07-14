package co.instil.surge.decoders;


import android.media.MediaCodec;

import java.io.IOException;

/**
 * Wrapper around the static initialisation methods of the {@link MediaCodec} class which
 * enables dependency injection of MediaCodec instances.
 */
public class MediaCodecFactory {

    public MediaCodec createDecoderOfType(String type) throws IOException {
        return MediaCodec.createDecoderByType(type);
    }

    public MediaCodec createEncoderOfType(String type) throws IOException {
        return MediaCodec.createEncoderByType(type);
    }

}
