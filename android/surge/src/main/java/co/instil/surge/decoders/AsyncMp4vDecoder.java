package co.instil.surge.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;
import co.instil.surge.client.SessionDescription;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public class AsyncMp4vDecoder extends MediaCodec.Callback implements Decoder {

    private static Logger logger = LoggerFactory.getLogger(AsyncMp4vDecoder.class);

    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private Surface surface;
    private List<ByteBuffer> decodeQueue = new ArrayList<>();
    private List<Integer> availableInputBuffers = new ArrayList<>();

    public AsyncMp4vDecoder(Surface surface) {
        this.surface = surface;
    }

    @Override
    public void decodeFrameBuffer(SessionDescription sessionDescription,
                                  ByteBuffer frameBuffer,
                                  int width,
                                  int height,
                                  int presentationTime,
                                  int duration) {

        try {
            logger.debug("Received MP4V frame for decoding");

            if (mediaCodec == null) {
                mediaCodec = createMediaCodec(sessionDescription, surface, width, height);
                mediaCodec.setCallback(this);
                mediaCodec.start();
            }

            if (availableInputBuffers.size() > 0) {
                logger.debug("Input buffer {} available, queueing frame for immediate decoding", availableInputBuffers.get(0));
                submitFrameToBuffer(frameBuffer, presentationTime, availableInputBuffers.remove(0), mediaCodec);
            } else {
                logger.debug("No input buffer available, queueing frame until one becomes available");
                decodeQueue.add(frameBuffer);
            }

        } catch (IllegalStateException e) {
            logger.error("Failed to get an input buffer from the media codec", e);
        } catch (IOException e) {
            logger.error("Device doesn't support MP4V decoding", e);
            throw new RuntimeException("Device doesn't support MP4V decoding");
        } catch (Exception e) {
            logger.error("Failed to decode MP4V", e);
        }
    }

    private MediaCodec createMediaCodec(SessionDescription sessionDescription, Surface surface, int width, int height) throws IOException {
        mediaCodec = MediaCodec.createDecoderByType(MediaFormat.MIMETYPE_VIDEO_MPEG4);
        mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_MPEG4, width, height);
        mediaFormat.setInteger(MediaFormat.KEY_WIDTH, width);
        mediaFormat.setInteger(MediaFormat.KEY_HEIGHT, height);
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
        mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(sessionDescription.getDecoderConfig().getBytes()));
        mediaCodec.configure(mediaFormat, surface, null, 0);
        return mediaCodec;
    }

    @Override
    public void close() throws InterruptedException {
        if (mediaCodec != null) {
            mediaCodec.stop();
            mediaCodec.release();
        }
    }

    private void submitFrameToBuffer(ByteBuffer frame, int presentationTime, int bufferIndex, MediaCodec codec) {
        logger.debug("Decode Queue Size {}", decodeQueue.size());
        ByteBuffer inputBuffer = codec.getInputBuffer(bufferIndex);

        byte[] fBuffer = new byte[frame.capacity()];
        frame.get(fBuffer);
        inputBuffer.put(fBuffer);

        int flags = 0;
        codec.queueInputBuffer(bufferIndex, 0, fBuffer.length, presentationTime, flags);
    }

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {
        logger.debug("Input buffer {} available", index);
        if (decodeQueue.size() == 0) {
            availableInputBuffers.add(index);
            return;
        }
        ByteBuffer frame = decodeQueue.remove(0);
        submitFrameToBuffer(frame, 1, index, codec);
    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
        logger.debug("Rendering and releasing output buffer");
        codec.releaseOutputBuffer(index, true);
    }

    @Override
    public void onError(MediaCodec codec, MediaCodec.CodecException e) {
        logger.debug("MP4V decoding failed", e);
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
        logger.debug("Output format changed");
    }
}
