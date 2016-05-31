package co.instil.surge.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Base64;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.List;

import co.instil.surge.decoders.nalu.NaluParser;
import co.instil.surge.decoders.nalu.NaluSegment;
import co.instil.surge.decoders.nalu.NaluType;
import co.instil.surge.logger.Logger;

/**
 *
 */
public class H264Decoder extends MediaCodec.Callback implements Decoder, Runnable {

    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private Thread decoderThread;
    private boolean running = true;
    private boolean configSet = false;

    public H264Decoder(Surface surface) {
        try {
            mediaCodec = createMediaCodec(surface);
            mediaCodec.start();

            decoderThread = new Thread(this);
            decoderThread.start();
        } catch (IOException e) {
            Logger.error("Device doesn't support H264 decoding " + Log.getStackTraceString(e));
            throw new RuntimeException("Device doesn't support H264 decoding");
        }
    }

    private MediaCodec createMediaCodec(Surface surface) throws IOException {
        mediaCodec = MediaCodec.createDecoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
        mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, 720, 480);
        mediaFormat.setInteger(MediaFormat.KEY_WIDTH, 720);
        mediaFormat.setInteger(MediaFormat.KEY_HEIGHT, 480);
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 720 * 480);

//        String offset = "\\x00\\x00\\x00\\x01";
//        String csd0 = offset + new String(Base64.decode("KO4CXLA=", Base64.DEFAULT), StandardCharsets.UTF_8);
//        String csd1 = offset + new String(Base64.decode("J2QAKKwrQFoe0A8SJqA=", Base64.DEFAULT), StandardCharsets.UTF_8);
//        mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd0.getBytes()));
//        mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd1.getBytes()));

        mediaCodec.configure(mediaFormat, surface, null, 0);
//        mediaCodec.setCallback(this);

        return mediaCodec;
    }

    @Override
    public void run() {
        while (running) {
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 500L);
//            Logger.debug("outputBufferIndex: " + outputBufferIndex);
            if (outputBufferIndex >= 0) {
                Logger.info("Decoder has available output");
                codecOuputBufferAvailable(mediaCodec, outputBufferIndex, bufferInfo);
            }
        }
    }

    private void codecOuputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
        codec.releaseOutputBuffer(index, true);
    }

    @Override
    public void decodeFrameBuffer(ByteBuffer frameBuffer, int duration, int presentationTime) {
        try {
            Logger.debug("Received frame buffer for decoding");

            List<NaluSegment> segments = NaluParser.parseNaluSegments(frameBuffer);

            if (!configSet && !hasSpsConfig(segments)) {
                return;
            }

            configSet = true;
            int index = mediaCodec.dequeueInputBuffer(600L);
            Logger.debug("index " + index);
            if (index >= 0) {
                ByteBuffer buffer = mediaCodec.getInputBuffer(index);
                buffer.clear();
                buffer.put(frameBuffer);
                mediaCodec.queueInputBuffer(index, 0, buffer.remaining(), 0, 0);
                Logger.debug("Decoding frame");
            } else {
                Logger.error("Failed to get an input buffer from the media codec");
            }
        } catch (IllegalStateException e) {
            Logger.error("Failed to get an input buffer from the media codec " + Log.getStackTraceString(e));
        } catch (Exception e) {
            Logger.error("Failed to get nalu segments " + Log.getStackTraceString(e));
        }
    }

    private boolean hasSpsConfig(List<NaluSegment> segments) {
        boolean containsSps = false;
        boolean containsPps = false;

        for (NaluSegment segment : segments) {
            if (segment.getType() == NaluType.SPS) {
                containsSps = true;
            } else if (segment.getType() == NaluType.PPS) {
                containsPps = true;
            }
        }

        return containsSps && containsPps;
    }

    @Override
    public void close() throws InterruptedException {
        running = false;
        if (decoderThread != null) {
            decoderThread.join();
        }

        if (mediaCodec != null) {
            mediaCodec.stop();
            mediaCodec.release();
        }
    }

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {
        Logger.debug("");
    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
        Logger.debug("");
    }

    @Override
    public void onError(MediaCodec codec, MediaCodec.CodecException e) {
        Logger.debug("");
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
        Logger.debug("");
    }
}
