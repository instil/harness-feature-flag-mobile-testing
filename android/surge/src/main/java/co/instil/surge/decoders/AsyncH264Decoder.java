package co.instil.surge.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;
import co.instil.surge.decoders.nalu.NaluParser;
import co.instil.surge.decoders.nalu.NaluSegment;
import co.instil.surge.decoders.nalu.NaluType;
import co.instil.surge.logger.Logger;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 *
 */
public class AsyncH264Decoder extends MediaCodec.Callback implements Decoder {

    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private Surface surface;
    private NaluSegment pictureParameterSet;
    private NaluSegment sequenceParameterSet;
    private List<Integer> availableInputBuffers = new ArrayList<>();

    private List<Frame> decodeQueue = new ArrayList<>();

    public AsyncH264Decoder(Surface surface) {
        this.surface = surface;
    }

    private MediaCodec createMediaCodec(Surface surface) throws IOException {
        mediaCodec = MediaCodec.createDecoderByType("video/avc");
        mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, 720, 480);
        mediaFormat.setInteger(MediaFormat.KEY_WIDTH, 720);
        mediaFormat.setInteger(MediaFormat.KEY_HEIGHT, 480);
        mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(sequenceParameterSet.getBuffer()));
        mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(pictureParameterSet.getBuffer()));
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 720 * 480);
        mediaCodec.configure(mediaFormat, surface, null, 0);
        return mediaCodec;
    }

    private class Frame {

        public long presentationTime;
        public NaluSegment segment;
        public Date timestamp;

        public Frame(NaluSegment segment, long presentationTime) {
            this.presentationTime = presentationTime;
            this.segment = segment;
            this.timestamp = new Date();
        }

        public boolean isKeyFrame() {
            return segment.getType() == NaluType.CODED_SLICE_IDR;
        }

        public boolean isSequenceParameterSet() {
            return segment.getType() == NaluType.SPS;
        }

        public boolean isPictureParameterSet() {
            return segment.getType() == NaluType.PPS;
        }

    }

    @Override
    public void decodeFrameBuffer(ByteBuffer frameBuffer, int duration, int presentationTime) {
        try {
            Logger.debug("Received frame buffer for decoding");

            List<NaluSegment> segments = NaluParser.parseNaluSegments(frameBuffer);

            if (!hasCachedParameterSets() && mediaCodec == null) {
                if (!containsParameterSets(segments)) {
                    return;
                }
                Logger.debug("Decoder received parameter sets");
                cacheParameterSets(segments);

                mediaCodec = createMediaCodec(surface);
                mediaCodec.setCallback(this);
                mediaCodec.start();
            }


            for (NaluSegment segment : segments) {
                Frame frame = new Frame(segment, presentationTime);

                if (availableInputBuffers.size() > 0) {
                    Logger.debug("Submitting to available buffer: " + availableInputBuffers.get(0));
                    submitFrameToBuffer(frame, availableInputBuffers.remove(0), mediaCodec);
                } else {
                    decodeQueue.add(frame);
                }
            }

        } catch (IllegalStateException e) {
            Logger.error("Failed to get an input buffer from the media codec " + Log.getStackTraceString(e));
        } catch (IOException e) {
            Logger.error("Device doesn't support H264 decoding " + Log.getStackTraceString(e));
            throw new RuntimeException("Device doesn't support H264 decoding");
        } catch (Exception e) {
            Logger.error("Failed to get nalu segments " + Log.getStackTraceString(e));
        }
    }

    private boolean containsParameterSets(List<NaluSegment> segments) {
        boolean containsSps = false, containsPps = false;
        for (NaluSegment segment : segments) {
            if (segment.getType() == NaluType.SPS) {
                containsSps = true;
            } else if (segment.getType() == NaluType.PPS) {
                containsPps = true;
            }
        }
        return containsSps && containsPps;
    }

    private boolean hasCachedParameterSets() {
        return pictureParameterSet != null && sequenceParameterSet != null;
    }

    private void cacheParameterSets(List<NaluSegment> segments) {
        for (NaluSegment segment : segments) {
            if (segment.getType() == NaluType.SPS) {
                sequenceParameterSet = segment;
            } else if (segment.getType() == NaluType.PPS) {
                pictureParameterSet = segment;
            }
        }
    }

    @Override
    public void close() throws InterruptedException {
        if (mediaCodec != null) {
            mediaCodec.stop();
            mediaCodec.release();
        }
    }

    private void submitFrameToBuffer(Frame frame, int bufferIndex, MediaCodec codec) {
        if (frame.segment == null) {
            return;
        }
        Logger.debug("Decode Queue Size: " + decodeQueue.size());
        ByteBuffer buffer = codec.getInputBuffer(bufferIndex);
        buffer.clear();
        buffer.put(frame.segment.getBuffer());

        Logger.debug(String.format("Submitting %s frame (%d) received at %s",
                frame.isKeyFrame() ? "key" : "non-key",
                frame.segment.getBufferSize(),
                frame.timestamp));

        int flags = 0;
        if (frame.isKeyFrame()) {
            flags = MediaCodec.BUFFER_FLAG_KEY_FRAME;
        } else if (frame.isPictureParameterSet() || frame.isSequenceParameterSet()) {
            flags = MediaCodec.BUFFER_FLAG_CODEC_CONFIG;
        }

        codec.queueInputBuffer(bufferIndex, 0, frame.segment.getBufferSize(), frame.presentationTime, flags);
    }


    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {
//        Logger.debug("onInputBufferAvailable!");
        if (decodeQueue.size() == 0) {
            availableInputBuffers.add(index);
            return;
        }
        Frame frame = decodeQueue.remove(0);
        submitFrameToBuffer(frame, index, codec);
    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
        Logger.debug("onOutputBufferAvailable");
        codec.releaseOutputBuffer(index, info.size > 0);
    }

    @Override
    public void onError(MediaCodec codec, MediaCodec.CodecException e) {
        Logger.debug("onError");
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
        Logger.debug("onOutputFormatChanged");
    }
}
