/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;
import co.instil.surge.decoders.h264.nalu.NaluSegment;
import co.instil.surge.decoders.h265.nalu.H265NaluSegment;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Wrapper around the static initialisation methods of the {@link MediaCodec} class which
 * enables dependency injection of MediaCodec instances.
 */
public class MediaCodecFactory {
    private static final String H264_DECODER_IDENTIFIER = "video/avc";
    private static final String H265_DECODER_IDENTIFIER = "video/hevc";
    private static final int DUMMY_WIDTH = 720;
    private static final int DUMMY_HEIGHT = 480;

    public MediaCodec createH264DecoderWithParameters(NaluSegment sps, NaluSegment pps, Surface surface) throws IOException {
        MediaCodec codec = MediaCodec.createDecoderByType(H264_DECODER_IDENTIFIER);
        MediaFormat format = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, DUMMY_WIDTH, DUMMY_HEIGHT);
        format.setByteBuffer("csd-0", ByteBuffer.wrap(sps.getPayload()));
        format.setByteBuffer("csd-1", ByteBuffer.wrap(pps.getPayload()));
        format.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 0);
        codec.configure(format, surface, null, 0);
        return codec;
    }

    public MediaCodec createH265DecoderWithParameters(H265NaluSegment vps, H265NaluSegment sps, H265NaluSegment pps, Surface surface) throws IOException {
        MediaCodec codec = MediaCodec.createDecoderByType(H265_DECODER_IDENTIFIER);
        MediaFormat format = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_HEVC, DUMMY_WIDTH, DUMMY_HEIGHT);
        format.setByteBuffer("csd-0", ByteBuffer.wrap(combineH265ParameterSets(vps, sps, pps)));
        format.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 0);
        codec.configure(format, surface, null, 0);
        return codec;
    }

    private byte[] combineH265ParameterSets(H265NaluSegment vps, H265NaluSegment sps, H265NaluSegment pps) {
        byte[] h265NaluSegmentsCombined = new byte[vps.getPayloadLength() + sps.getPayloadLength() + pps.getPayloadLength()];
        Arrays.fill(h265NaluSegmentsCombined, (byte) 0);
        System.arraycopy(vps.getPayload(), 0, h265NaluSegmentsCombined, 0, vps.getPayloadLength());
        System.arraycopy(sps.getPayload(), 0, h265NaluSegmentsCombined, vps.getPayloadLength(), sps.getPayloadLength());
        System.arraycopy(pps.getPayload(), 0, h265NaluSegmentsCombined, vps.getPayloadLength() + sps.getPayloadLength(), pps.getPayloadLength());
        return h265NaluSegmentsCombined;
    }

    public MediaCodec createDecoderOfType(String type) throws IOException {
        return MediaCodec.createDecoderByType(type);
    }

    public MediaCodec createEncoderOfType(String type) throws IOException {
        return MediaCodec.createEncoderByType(type);
    }

}
