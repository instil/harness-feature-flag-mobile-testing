/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.h264;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.media.MediaFormat;

import java.util.ArrayList;
import java.util.List;

import co.instil.surge.client.SurgeVideoView;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

/**
 * An H264 stream decoder which utilises native hardware decoders (if available) and the
 * asynchronous {@link MediaCodec} Android platform APIs.
 * <p>
 * The {@link AsyncH264Decoder} is supported on Android Lollipop (5.0) and above only as it
 * relies on MediaCodec APIs available in API level 21 and above.  In order to support order
 * devices please refer to the {@link SyncH264Decoder}.
 *
 * @see MediaCodec
 * @see SyncH264Decoder
 */
@TargetApi(21)
public class AsyncH264Decoder extends H264Decoder {
    private static final Logger LOGGER = LoggerFactory.getLogger(AsyncH264Decoder.class);

    private List<Integer> availableInputBuffers = new ArrayList<>();
    private List<H264Packet> decodeQueue = new ArrayList<>();

    /**
     * Constrcuts a new {@link AsyncH264Decoder}.
     * @param videoView the view onto which the decoded stream will be rendered.
     */
    public AsyncH264Decoder(SurgeVideoView videoView) {
        super(videoView);
    }

    @Override
    public void onCreatedMediaCodec(MediaCodec mediaCodec) {
        mediaCodec.setCallback(new AsyncH264DecoderCallbackHandler());
    }

    @Override
    protected void onStartedCodec(MediaCodec mediaCodec) { }

    @Override
    protected void onReceiveH264Packet(H264Packet packet) {
        if (!availableInputBuffers.isEmpty()) {
            LOGGER.debug("Submitting to available buffer: {}", availableInputBuffers.get(0));
            submitToDecoder(packet, availableInputBuffers.remove(0), getMediaCodec());
        } else {
            decodeQueue.add(packet);
        }
    }

    private void submitToDecoder(H264Packet packet, int bufferIndex, MediaCodec codec) {
        if (packet.segment == null) {
            return;
        }
        LOGGER.debug("Decode Queue Size: {}", decodeQueue.size());
        writePacketToInputBuffer(packet, bufferIndex);
        LOGGER.debug("Submitting to decoder: {}", packet.toString());
        int flags = decoderFlagsForPacket(packet);
        codec.queueInputBuffer(bufferIndex, 0, packet.segment.getPayloadLength(), packet.presentationTime, flags);
    }

    private class AsyncH264DecoderCallbackHandler extends MediaCodec.Callback {

        @Override
        public void onInputBufferAvailable(MediaCodec codec, int index) {
            if (decodeQueue.isEmpty()) {
                availableInputBuffers.add(index);
                return;
            }
            H264Packet packet = decodeQueue.remove(0);

            try {
                submitToDecoder(packet, index, codec);
            } catch (Exception e) {
                LOGGER.error("Failed to decode H264 frame: " + e.toString());
                LOGGER.printStackTrace(e);
            }
        }

        @Override
        public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
            try {
                codec.releaseOutputBuffer(index, info.size > 0);
            } catch (Exception e) {
                LOGGER.error("Failed to release output buffer: " + e.toString());
                LOGGER.printStackTrace(e);
            }
        }

        @Override
        public void onError(MediaCodec codec, MediaCodec.CodecException exception) { }

        @Override
        public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
            setMediaFormat(format);
        }

    }

}
