package co.instil.surge.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Build;
import android.view.Surface;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.nalu.NaluParser;
import co.instil.surge.decoders.nalu.NaluSegment;
import co.instil.surge.decoders.nalu.NaluType;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Date;
import java.util.List;

/**
 * Abstract superclass for all H264 decoders which implements common operations e.g. parsing
 * of byte buffers from the {@link co.instil.surge.player.RtspPlayer} into NAL units and
 * codec intialization.
 *
 * @see AsyncH264Decoder
 * @see SyncH264Decoder
 */
public abstract class H264Decoder implements Decoder {

    private static Logger logger = LoggerFactory.getLogger(H264Decoder.class);

    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private Surface surface;
    private NaluSegment pictureParameterSet;
    private NaluSegment sequenceParameterSet;
    private ByteBuffer[] inputBuffers = null;


    /**
     * Constructor for a {@link H264Decoder} instance.
     * @param surface the surface into which the decoder will return the decoded video.
     */
    protected H264Decoder(Surface surface) {
        setSurface(surface);
    }

    @Override
    public void decodeFrameBuffer(SessionDescription sessionDescription,
                                  ByteBuffer frameBuffer,
                                  int width,
                                  int height,
                                  int presentationTime,
                                  int duration) {

        logger.debug("Received frame buffer for decoding");

        try {
            List<NaluSegment> segments = NaluParser.parseNaluSegments(frameBuffer);

            MediaCodec codec = getMediaCodec();
            if (!hasCachedParameterSets() && codec == null) {
                if (!containsParameterSets(segments)) {
                    return;
                }
                logger.debug("Decoder received parameter sets");
                cacheParameterSets(segments);

                codec = createMediaCodec(getSurface());
                codec.start();
                if (isPreLollipopDevice()) {
                    inputBuffers = codec.getInputBuffers();
                }
                onStartedCodec(codec);
            }

            for (NaluSegment unit : segments) {
                onReceiveH264Packet(new H264Packet(unit, presentationTime));
            }

        } catch (Exception ex) {

        }
    }

    @Override
    public void close() throws InterruptedException {
        MediaCodec mediaCodec = getMediaCodec();
        if (mediaCodec != null) {
            mediaCodec.stop();
            mediaCodec.release();
        }
    }

    /**
     * Invoked after the {@link MediaCodec} has been intialised (but not started) by the
     * {@link H264Decoder}; allows subclasses to customise the MediaCodec.
     * @param mediaCodec the newly created MediaCodec.
     */
    protected abstract void onCreatedMediaCodec(MediaCodec mediaCodec);

    /**
     * Invoked after the {@link MediaCodec} has been started (but not started) by the
     * {@link H264Decoder}; allows subclasses to add behaviour.
     * @param mediaCodec the started MediaCodec.
     */
    protected abstract void onStartedCodec(MediaCodec mediaCodec);

    /**
     * Invoked when the {@link H264Decoder} receives a {@link H264Packet} to be decoded.
     * <p>
     * The base {@link H264Decoder} will not decode the packet; subclasses should implement
     * this method to invoke the appropriate {@link MediaCodec} methods to handle the decoding.
     * @param packet the packet of H264 video data to be decoded.
     */
    protected abstract void onReceiveH264Packet(H264Packet packet);

    /**
     * Determines whether the list of {@link NaluSegment} contains the 'Sequence Paramater Set'
     * (SPS) and 'Picture Parameter Set' (PPS) NAL units; which are used to configure the decoder.
     * @param segments the segments to be inspected for SPS / PPS data.
     * @return true if the list contains SPS and PPS NAL units; false otherwise.
     */
    protected boolean containsParameterSets(List<NaluSegment> segments) {
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

    /**
     * Determines whether the decoder has yet encountered and stored 'Sequence Paramater Set'
     * (SPS) and 'Picture Parameter Set' (PPS) NAL units.
     * @return true if the decoder has been initialised with SPS and PSS.
     */
    protected boolean hasCachedParameterSets() {
        return pictureParameterSet != null && sequenceParameterSet != null;
    }

    private void cacheParameterSets(List<NaluSegment> segments) {
        for (NaluSegment segment : segments) {
            if (segment.getType() == NaluType.SPS) {
                setSequenceParameterSet(segment);
            } else if (segment.getType() == NaluType.PPS) {
                setPictureParameterSet(segment);
            }
        }
    }

    protected void writePacketToInputBuffer(H264Packet packet, int inputBufferId) {
        ByteBuffer buffer;
        if (inputBufferId == -1) {
            return;
        }
        if (isPreLollipopDevice()) {
            buffer = inputBuffers[inputBufferId];
        } else {
            buffer = mediaCodec.getInputBuffer(inputBufferId);
        }
        buffer.clear();
        buffer.put(packet.segment.getBuffer());
    }

    protected void setPictureParameterSet(NaluSegment segment) {
        this.pictureParameterSet = segment;
    }

    protected int decoderFlagsForPacket(H264Packet packet) {
        int flags = 0;
        if (packet.isKeyFrame()) {
            flags &= MediaCodec.BUFFER_FLAG_KEY_FRAME;
        }
        if (packet.isPictureParameterSet() || packet.isSequenceParameterSet()) {
            flags &= MediaCodec.BUFFER_FLAG_CODEC_CONFIG;
        }
        return 0;
    }

    protected NaluSegment getPictureParameterSet() {
        return pictureParameterSet;
    }

    protected NaluSegment getSequenceParameterSet() {
        return sequenceParameterSet;
    }

    protected void setSequenceParameterSet(NaluSegment segment) {
        this.sequenceParameterSet = segment;
    }

    protected void setMediaCodec(MediaCodec mediaCodec) {
        this.mediaCodec = mediaCodec;
    }

    protected MediaCodec getMediaCodec() {
        return mediaCodec;
    }

    protected void setMediaFormat(MediaFormat mediaFormat) {
        this.mediaFormat = mediaFormat;
    }

    protected MediaFormat getMediaFormat() {
        return mediaFormat;
    }

    protected void setSurface(Surface surface) {
        this.surface = surface;
    }

    protected Surface getSurface() {
        return surface;
    }

    /**
     * A wrapper around a single H264 timestamp which provides additional meta-data and
     * utility methods.
     */
    protected class H264Packet {

        public long presentationTime;
        public NaluSegment segment;
        public Date timestamp;

        public H264Packet(NaluSegment segment, long presentationTime) {
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

        public String toString() {
            return String.format("H264 packet: %s frame of size %d bytes received at %s",
                    isKeyFrame() ? "key" : "non-key",
                    segment.getBufferSize(),
                    timestamp);
        }

    }

    private boolean isPreLollipopDevice() {
        return Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP;
    }

    private MediaCodec createMediaCodec(Surface surface) throws IOException {
        setMediaCodec( MediaCodec.createDecoderByType("video/avc"));
        setMediaFormat(MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, 720, 480));
        getMediaFormat().setInteger(MediaFormat.KEY_WIDTH, 720);
        getMediaFormat().setInteger(MediaFormat.KEY_HEIGHT, 480);
        getMediaFormat().setByteBuffer("csd-0", ByteBuffer.wrap(getSequenceParameterSet().getBuffer()));
        getMediaFormat().setByteBuffer("csd-1", ByteBuffer.wrap(getPictureParameterSet().getBuffer()));
        getMediaFormat().setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 0);
        getMediaCodec().configure(getMediaFormat(), surface, null, 0);
        onCreatedMediaCodec(getMediaCodec());
        return getMediaCodec();
    }

}
