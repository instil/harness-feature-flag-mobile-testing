package co.instil.surge.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.nalu.NaluParser;
import co.instil.surge.decoders.nalu.NaluSegment;
import co.instil.surge.decoders.nalu.NaluType;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 *
 */
public class AsyncH264Decoder extends MediaCodec.Callback implements Decoder {

    private static Logger logger = LoggerFactory.getLogger(AsyncH264Decoder.class);

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

    private MediaCodec createMediaCodec(Surface surface, int width, int height) throws IOException {
        mediaCodec = MediaCodec.createDecoderByType("video/avc");
        mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
        mediaFormat.setInteger(MediaFormat.KEY_WIDTH, width);
        mediaFormat.setInteger(MediaFormat.KEY_HEIGHT, height);
        mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(sequenceParameterSet.getBuffer()));
        mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(pictureParameterSet.getBuffer()));
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
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
    public void decodeFrameBuffer(SessionDescription sessionDescription,
                                  ByteBuffer frameBuffer,
                                  int width,
                                  int height,
                                  int presentationTime,
                                  int duration) {

        try {
            logger.debug("Received H264 frame for decoding");

            List<NaluSegment> segments = NaluParser.parseNaluSegments(frameBuffer);

            if (!hasCachedParameterSets() && mediaCodec == null) {
                if (!containsParameterSets(segments)) {
                    return;
                }
                logger.debug("Decoder received parameter sets");
                cacheParameterSets(segments);

                mediaCodec = createMediaCodec(surface, width, height);
                mediaCodec.setCallback(this);
                mediaCodec.start();
            }


            for (NaluSegment segment : segments) {
                Frame frame = new Frame(segment, presentationTime);

                if (availableInputBuffers.size() > 0) {
                    logger.debug("Submitting to available buffer: " + availableInputBuffers.get(0));
                    submitFrameToBuffer(frame, availableInputBuffers.remove(0), mediaCodec);
                } else {
                    decodeQueue.add(frame);
                }
            }

        } catch (IllegalStateException e) {
            logger.error("Failed to get an input buffer from the media codec", e);
        } catch (IOException e) {
            logger.error("Device doesn't support H264 decoding ", e);
            throw new RuntimeException("Device doesn't support H264 decoding");
        } catch (Exception e) {
            logger.error("Failed to get nalu segments ", e);
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
        logger.debug("Decode Queue Size: " + decodeQueue.size());
        ByteBuffer buffer = codec.getInputBuffer(bufferIndex);
        buffer.clear();
        buffer.put(frame.segment.getBuffer());

        logger.debug("Submitting {} frame ({}) received at {}",
                frame.isKeyFrame() ? "key" : "non-key",
                frame.segment.getBufferSize(),
                frame.timestamp);

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
        if (decodeQueue.size() == 0) {
            availableInputBuffers.add(index);
            return;
        }
        Frame frame = decodeQueue.remove(0);
        submitFrameToBuffer(frame, index, codec);
    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
        logger.debug("Rendering and releasing output buffer");
        codec.releaseOutputBuffer(index, info.size > 0);
    }

    @Override
    public void onError(MediaCodec codec, MediaCodec.CodecException e) {
        logger.debug("H264 decoding failed", e);
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
        logger.debug("Output format changed");
    }
}
