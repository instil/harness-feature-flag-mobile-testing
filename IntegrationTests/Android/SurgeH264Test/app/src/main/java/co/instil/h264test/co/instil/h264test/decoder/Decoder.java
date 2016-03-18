package co.instil.h264test.co.instil.h264test.decoder;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;

/**
 * Created by redbrain on 14/03/2016.
 */
public class Decoder implements Runnable {
    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private NaluSegment lastParameterSet[];
    private Thread decoderThread;

    private boolean running;

    private final int kBufferingTimeoutMS = 5000;

    private final String TAG = "H264Decoder";

    // CTORS
    public Decoder() {
        this.lastParameterSet = new NaluSegment[2];
    }

    /**
     * Decoder the specified contiguous block of H.264 NAL units.
     *
     * @param  buffer  Buffer containing H264 NAL units.
     */
    public void decodeFrameBuffer(byte[] buffer) {
        // Get the NAL unit segments from the buffer.
        /*List<NaluSegment> segments = null;
        try {
            // parse raw nalus
            segments = NaluParser.parseNaluSegments(buffer);
        } catch (Exception e){
            return;
        }

        // send nalus to decoder queue
        for (NaluSegment segment : segments) {

            // ignore unspecified NAL units.
            if (segment.getType() != NaluType.UNSPECIFIED) {

                // Hold the parameter set for stop/start initialization speed
                if (segment.getType() == NaluType.PPS) {
                    lastParameterSet[0] = segment;
                } else if (segment.getType() == NaluType.SPS) {
                    lastParameterSet[1] = segment;
                }

            }
        }*/

        //synchronized (this) {
            if (!running) {
                return;
            }

            int index = mediaCodec.dequeueInputBuffer(600);
            if (index >= 0) {
                codecInputBufferAvailable(buffer, buffer.length, mediaCodec, index);
            } else {
                Log.i("DECODER", "input timeout!");
            }
        //}
    }

    /**
     * Start the Decoder does nothing if already running.
     *
     * NOTE: we try to hold onto the media codec for stop/start for surface changes.
     *       Close and Start must be called for a full clean
     */
    public boolean startDecoder(Surface targetSurface) {

        Log.d(TAG, "Starting Decoder");
        synchronized (this) {

            try {
                mediaCodec = MediaCodec.createDecoderByType(MediaFormat.MIMETYPE_VIDEO_MPEG4);
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }

            // check if media codec is null handles the mocked android unit-tests case.
            if (mediaCodec != null) {
                mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_MPEG4, 1280, 720);

                // try and set the media format directly from old parameters
                // configureH264ParameterSetsFromLastParameterSet();

                // configure decoder with target surface
                mediaCodec.configure(mediaFormat, targetSurface, null, 0);

                // start the decoder
                mediaCodec.start();
                running = true;
            }

            // start async decoder loop
            decoderThread = new Thread(this);
            decoderThread.start();

        }
        return true;
    }

    /**
     * Configure media format from preserved parameter sets
     */
    private void configureH264ParameterSetsFromLastParameterSet() {
        if (lastParameterSet[0] != null && lastParameterSet[1] != null) {
            // NaluType.PPS
            configureCodecWithPictureParameter(lastParameterSet[0]);
            // NaluType.SPS
            configureCodecWithSequenceParameter(lastParameterSet[1]);
        }
    }

    private void configureCodecWithSequenceParameter(NaluSegment sequenceParameter) {
        mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(sequenceParameter.getBuffer()));
    }

    private void configureCodecWithPictureParameter(NaluSegment pictureParameter) {
        mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(pictureParameter.getBuffer()));
    }


    public void stopDecoder() throws InterruptedException {
        if (running) {
            // stop
            running = false;
            // wait untill async decoder stops
            decoderThread.join();
            // stop media codec
            mediaCodec.stop();
        }
    }

    public void closeDecoder() throws InterruptedException {
        stopDecoder();
        if (mediaCodec != null) {
            mediaCodec.release();
            mediaCodec = null;
            mediaFormat = null;
        }
    }

    // HOMEBREW ASYNC DECODER LOOP
    @Override
    public void run() {
        // mediaCodec input + output dequeue timeouts
        long kOutputBufferTimeoutMs = 500;

        while (running) {
            // output any video frames
            //synchronized (this) {
                MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
                // always check if output is available.
                int outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, kOutputBufferTimeoutMs);
                if (outputBufferIndex >= 0) {
                    // Try and render first
                    Log.i("DECODER", "OUTPUT AVAILABLE");
                    codecOuputBufferAvailable(mediaCodec, outputBufferIndex, info);
                }
            //}
        }
    }

    // CODEC INPUT
    private void codecInputBufferAvailable(byte[] videoBuffer, int length, MediaCodec codec, int index) {
        ByteBuffer[] buffers = codec.getInputBuffers();
        ByteBuffer buffer = buffers[index];
        buffer.clear();
        buffer.put(videoBuffer);
        codec.queueInputBuffer(index, 0, length, 0, 0);
    }

    // CODEC OUTPUT
    private void codecOuputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
        codec.releaseOutputBuffer(index, true);
    }

}
