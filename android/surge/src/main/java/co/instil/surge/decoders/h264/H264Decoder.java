/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.h264;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.Looper;

import co.instil.surge.client.SessionDescription;
import co.instil.surge.client.SurgeVideoView;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.decoders.MediaCodecFactory;
import co.instil.surge.decoders.h264.nalu.NaluParser;
import co.instil.surge.decoders.h264.nalu.NaluSegment;
import co.instil.surge.decoders.h264.nalu.NaluType;
import co.instil.surge.diagnostics.DiagnosticsTracker;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;

/**
 * Abstract superclass for all H264 decoders which implements common operations e.g. parsing
 * of byte buffers from the {@link co.instil.surge.player.SurgeRtspPlayer} into NAL units and
 * codec intialization.
 *
 * @see AsyncH264Decoder
 */
public abstract class H264Decoder implements Decoder {
    private static final Logger LOGGER = LoggerFactory.getLogger(H264Decoder.class);
    private static final String MEDIA_FORMAT_WIDTH_KEY = "width";
    private static final String MEDIA_FORMAT_HEIGHT_KEY = "height";

    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private SurgeVideoView videoView;
    private MediaCodecFactory mediaCodecFactory;
    private NaluParser naluParser;
    private NaluSegment pictureParameterSet;
    private NaluSegment sequenceParameterSet;
    private H264Packet lastSPSPacket = null;
    private H264Packet lastPPSPacket = null;
    private DiagnosticsTracker diagnostics;

    /**
     * Constructor for a {@link H264Decoder} instance.
     * @param videoView the surface into which the decoder will return the decoded video.
     */
    protected H264Decoder(SurgeVideoView videoView, DiagnosticsTracker diagnosticsTracker) {
        this(videoView, new MediaCodecFactory(), new NaluParser(), diagnosticsTracker);
    }

    /**
     * Constructor for a {@link H264Decoder} instance.
     * @param videoView the surface into which the decoder will return the decoded video.
     * @param mediaCodecFactory factory used to instantiate {@link MediaCodec} decoder interfaces.
     * @param naluParser parser used to parse NAL units from raw packets of bytes handed to the decoder.
     */
    protected H264Decoder(
            SurgeVideoView videoView,
            MediaCodecFactory mediaCodecFactory,
            NaluParser naluParser,
            DiagnosticsTracker diagnosticsTracker) {
        setVideoView(videoView);
        this.mediaCodecFactory = mediaCodecFactory;
        this.naluParser = naluParser;
        this.diagnostics = diagnosticsTracker;
    }

    @Override
    public void decodeFrameBuffer(SessionDescription sessionDescription,
                                  ByteBuffer frameBuffer,
                                  int width,
                                  int height,
                                  int presentationTime,
                                  int duration) {

        LOGGER.debug("Received frame buffer for decoding");

        diagnostics.trackNewFrameOfSize(frameBuffer.remaining() * 8);

        try {
            List<NaluSegment> segments = naluParser.parseNaluSegments(frameBuffer);

            MediaCodec codec = getMediaCodec();
            if (!hasCachedParameterSets() && codec == null) {
                if (!containsParameterSets(segments)) {
                    return;
                }
                LOGGER.debug("Decoder received parameter sets");
                cacheParameterSets(segments);

                codec = createMediaCodec();
                setMediaCodec(codec);
                onCreatedMediaCodec(codec);
                codec.start();
                onStartedCodec(codec);
            }

            for (NaluSegment unit : segments) {
                H264Packet packet = new H264Packet(unit, presentationTime);
                if (packet.isKeyFrame()) {
                    if (lastSPSPacket != null) {
                        onReceiveH264Packet(lastSPSPacket);
                    }
                    if (lastPPSPacket != null) {
                        onReceiveH264Packet(lastPPSPacket);
                    }
                    onReceiveH264Packet(packet);
                } else if (packet.isSequenceParameterSet()) {
                    lastSPSPacket = packet;
                } else if (packet.isPictureParameterSet()) {
                    lastPPSPacket = packet;
                } else {
                    onReceiveH264Packet(packet);
                }
            }

        } catch (Exception ex) {
            LOGGER.error("Failed to decode frame", ex);
        }
    }

    @Override
    public void close() throws InterruptedException {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                MediaCodec mediaCodec = getMediaCodec();
                if (mediaCodec != null) {
                    try {
                        mediaCodec.stop();
                        mediaCodec.release();
                    } catch (Exception e) {
                        LOGGER.error("Failed to close MediaCodec", e);
                    }
                }
            }
        });
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
        if (inputBufferId == -1 || packet == null) {
            return;
        }

        ByteBuffer buffer;
        buffer = mediaCodec.getInputBuffer(inputBufferId);
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
        if (mediaFormatHasStreamDimensions(mediaFormat)) {
            int streamWidth = mediaFormat.getInteger(MEDIA_FORMAT_WIDTH_KEY);
            int streamHeight = mediaFormat.getInteger(MEDIA_FORMAT_HEIGHT_KEY);
            videoView.setVideoDimensions(streamWidth, streamHeight);
            diagnostics.trackNewFrameDimensions(streamWidth, streamHeight);
        }
    }

    private boolean mediaFormatHasStreamDimensions(MediaFormat mediaFormat) {
        return mediaFormat.containsKey(MEDIA_FORMAT_WIDTH_KEY) && mediaFormat.containsKey(MEDIA_FORMAT_HEIGHT_KEY);
    }

    protected MediaFormat getMediaFormat() {
        return mediaFormat;
    }

    protected void setVideoView(SurgeVideoView videoView) {
        this.videoView = videoView;
    }

    protected SurgeVideoView getVideoView() {
        return videoView;
    }

    private MediaCodec createMediaCodec() throws IOException {
        return mediaCodecFactory.createH264DecoderWithParameters(
                sequenceParameterSet,
                pictureParameterSet,
                videoView.generateUniqueSurface());
    }
}
