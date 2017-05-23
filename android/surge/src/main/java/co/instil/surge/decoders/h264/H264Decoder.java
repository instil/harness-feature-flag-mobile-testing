/*
 * Copyright (c) 2016 Instil Software.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

package co.instil.surge.decoders.h264;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;

import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.decoders.MediaCodecFactory;
import co.instil.surge.decoders.h264.nalu.NaluParser;
import co.instil.surge.decoders.h264.nalu.NaluSegment;
import co.instil.surge.decoders.h264.nalu.NaluType;
import co.instil.surge.device.DeviceExaminer;

/**
 * Abstract superclass for all H264 decoders which implements common operations e.g. parsing
 * of byte buffers from the {@link co.instil.surge.player.RtspPlayer} into NAL units and
 * codec intialization.
 *
 * @see AsyncH264Decoder
 * @see SyncH264Decoder
 */
@TargetApi(21)
public abstract class H264Decoder implements Decoder {


    private static Logger logger = LoggerFactory.getLogger(H264Decoder.class);

    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private Surface surface;
    private MediaCodecFactory mediaCodecFactory;
    private NaluParser naluParser;
    private NaluSegment pictureParameterSet;
    private NaluSegment sequenceParameterSet;
    private ByteBuffer[] inputBuffers = null;
    private DeviceExaminer deviceExaminer;


    /**
     * Constructor for a {@link H264Decoder} instance.
     * @param surface the surface into which the decoder will return the decoded video.
     */
    protected H264Decoder(Surface surface) {
        this(surface, new MediaCodecFactory(), new NaluParser(), new DeviceExaminer());
    }

    /**
     * Constructor for a {@link H264Decoder} instance.
     * @param surface the surface into which the decoder will return the decoded video.
     * @param mediaCodecFactory factory used to instantiate {@link MediaCodec} decoder interfaces.
     * @param naluParser parser used to parse NAL units from raw packets of bytes handed to the decoder.
     */
    protected H264Decoder(
            Surface surface,
            MediaCodecFactory mediaCodecFactory,
            NaluParser naluParser,
            DeviceExaminer deviceExaminer) {
        setSurface(surface);
        this.mediaCodecFactory = mediaCodecFactory;
        this.naluParser = naluParser;
        this.deviceExaminer = deviceExaminer;
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
            List<NaluSegment> segments = naluParser.parseNaluSegments(frameBuffer);

            MediaCodec codec = getMediaCodec();
            if (!hasCachedParameterSets() && codec == null) {
                if (!containsParameterSets(segments)) {
                    return;
                }
                logger.debug("Decoder received parameter sets");
                cacheParameterSets(segments);

                codec = createMediaCodec();
                setMediaCodec(codec);
                onCreatedMediaCodec(codec);
                codec.start();
                if (deviceExaminer.isPreLollipopDevice()) {
                    inputBuffers = codec.getInputBuffers();
                }
                onStartedCodec(codec);
            }

            for (NaluSegment unit : segments) {
                onReceiveH264Packet(new H264Packet(unit, presentationTime));
            }

        } catch (Exception ex) {
            logger.error("Failed to decode frame", ex);
        }
    }

    @Override
    public void close() throws InterruptedException {
        MediaCodec mediaCodec = getMediaCodec();
        if (mediaCodec != null) {
            try {
                mediaCodec.stop();
                mediaCodec.release();
            } catch (Exception e) {
                logger.error("Failed to close MediaCodec", e);
            }
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
        if (inputBufferId == -1 || packet == null) {
            return;
        }
        if (deviceExaminer.isPreLollipopDevice()) {
            buffer = mediaCodec.getInputBuffers()[inputBufferId]; //inputBuffers[inputBufferId];
        } else {
            buffer = mediaCodec.getInputBuffer(inputBufferId);
        }
        buffer.clear();
        byte[] payload = packet.segment.getPayload();
        if (payload != null && payload.length > 0) {
            buffer.put(payload, 0, payload.length);
        }
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
        return flags;
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

    private MediaCodec createMediaCodec() throws IOException {
        return mediaCodecFactory.createH264DecoderWithParameters(
                sequenceParameterSet, pictureParameterSet, surface, 720, 480);
    }


}
