package co.instil.surge.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.provider.MediaStore;
import android.view.Surface;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 *
 */
public class Mp4vDecoder extends MediaCodec.Callback implements Decoder {

    private static final Logger logger = LoggerFactory.getLogger(Mp4vDecoder.class);

    private final Surface targetSurface;

    private MediaCodec mediaCodec;

    public Mp4vDecoder(Surface targetSurface) {
        this.targetSurface = targetSurface;
    }

    @Override
    public void decodeFrameBuffer(ByteBuffer frameBuffer, int duration, int presentationTime) {
        if (mediaCodec == null) {
            createMediaCodec();
        }
    }

    private void createMediaCodec() {
        try {
            mediaCodec = MediaCodec.createByCodecName(MediaFormat.MIMETYPE_VIDEO_MPEG4);
        } catch (IOException e) {
            logger.error("Failed to create decoder", e);
            return;
        }

        MediaFormat mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_MPEG4, 720, 480);
        mediaCodec.configure(mediaFormat, targetSurface, null, 0);
        mediaCodec.setCallback(this);
        mediaCodec.start();
    }

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {

    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {

    }

    @Override
    public void onError(MediaCodec codec, MediaCodec.CodecException e) {

    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {

    }
}
