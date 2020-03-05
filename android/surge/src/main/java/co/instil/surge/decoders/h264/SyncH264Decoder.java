/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.h264;

import android.annotation.TargetApi;
import android.media.MediaCodec;

import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.TimeUnit;

import co.instil.surge.client.SurgeVideoView;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

/**
 * An H264 stream decoder which utilises native hardware decoders (if available) and the
 * synchronous {@link MediaCodec} Android platform APIs.
 * <p>
 *
 * @see MediaCodec
 * @see AsyncH264Decoder
 */
@TargetApi(19)
public class SyncH264Decoder extends H264Decoder {
    private static final Logger LOGGER = LoggerFactory.getLogger(SyncH264Decoder.class);

    private Thread decodedPacketConsumer;
    private Thread packetQueueConsumer;
    private PacketQueueConsumer consumer;
    private boolean isRunning = true;

    /**
     * Constructor for {@link SyncH264Decoder} instances.
     * @param videoView the view onto which the decoded stream will be rendered.
     */
    public SyncH264Decoder(SurgeVideoView videoView) {
        super(videoView);
        consumer = new PacketQueueConsumer(100);
        packetQueueConsumer = new Thread(consumer);
        decodedPacketConsumer = new Thread(new DecodedFrameConsumer(100));
    }

    @Override
    protected void onCreatedMediaCodec(MediaCodec mediaCodec) { }

    @Override
    protected void onStartedCodec(MediaCodec mediaCodec) {
        packetQueueConsumer.start();
        decodedPacketConsumer.start();
    }

    @Override
    protected void onReceiveH264Packet(H264Packet packet) {
        if (isRunning) {
            consumer.queueEncodedPacket(packet);
        }
    }

    @Override
    public void close() throws InterruptedException {
        isRunning = false;

        while (decodedPacketConsumer.isAlive() || packetQueueConsumer.getState() == Thread.State.RUNNABLE) {
            Thread.sleep(10);
        }

        super.close();
    }

    /**
     * Consumes the FIFO (first-in-first-out) queue of packets being submitted to the decoder and
     * writes them to the {@link MediaCodec} input buffers.
     */
    private class PacketQueueConsumer implements Runnable {

        private final int millisecondsTimeout;

        private LinkedBlockingDeque<H264Packet> packetQueue;

        public PacketQueueConsumer(int millisecondsTimeout) {
            this.millisecondsTimeout = millisecondsTimeout;
            packetQueue = new LinkedBlockingDeque<>();
        }

        protected void queueEncodedPacket(H264Packet packet) {
            packetQueue.add(packet);
            LOGGER.debug("Queued packet, new queue length: {}", packetQueue.size());
        }

        @Override
        public void run() {
            H264Packet packet;
            while (isRunning) {
                packet = null;
                try {
                    packet = packetQueue.pollFirst(millisecondsTimeout, TimeUnit.MILLISECONDS);
                } catch (InterruptedException ex) {}

                if (packet != null) {
                    int bufferId = getMediaCodec().dequeueInputBuffer(0);

                    if (bufferId == -1) {
                        LOGGER.error("Dropped frame: H264 codec not ready");
                        continue;
                    }

                    if (packet.segment == null) {
                        LOGGER.info("!");
                        return;
                    }

                    writePacketToInputBuffer(packet, bufferId);

                    LOGGER.debug("Submitting to decoder: {}", packet.toString());
                    int flags = decoderFlagsForPacket(packet);

                    try {
                        getMediaCodec().queueInputBuffer(
                                bufferId, 0, packet.segment.getPayloadLength(), packet.presentationTime, flags);

                    } catch (IllegalStateException e) {
                        LOGGER.error("H264 Codec has stopped processing, exiting and closing decoder.");
                    }
                }
            }
        }

    }

    private class DecodedFrameConsumer implements Runnable {

        private final int millisecondsTimeout;

        public DecodedFrameConsumer(int millisecondsTimeout) {
            this.millisecondsTimeout = millisecondsTimeout;
        }

        @Override
        public void run() {
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            try {
                while (getMediaCodec() != null && isRunning) {
                    int bufferId = getMediaCodec().dequeueOutputBuffer(bufferInfo, millisecondsTimeout);
                    if (bufferId >= 0) {
                        LOGGER.warn("DecodedFrameConsumer got buffer ID: {}", bufferId);
                        getMediaCodec().releaseOutputBuffer(bufferId, bufferInfo.size > 0);
                    } else if (bufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                        LOGGER.debug("DecodedFrameConsumer got buffer ID: {}", bufferId);
                        setMediaFormat(getMediaCodec().getOutputFormat());
                    }
                }

            } catch (Exception e) {
                LOGGER.error("Failed to release output buffer: " + e.toString());
                LOGGER.printStackTrace(e);
            }
        }
    }

}
