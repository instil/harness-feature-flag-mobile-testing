package co.instil.surge.decoders;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.view.Surface;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.concurrent.LinkedBlockingDeque;


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

    private static Logger logger = LoggerFactory.getLogger(SyncH264Decoder.class);

    private Thread decodedPacketConsumer;
    private Thread packetQueueConsumer;
    private PacketQueueConsumer consumer;

    /**
     * Constructor for {@link SyncH264Decoder} instances.
     * @param surface the surface onto which the decoded stream will be rendered.
     */
    public SyncH264Decoder(Surface surface) {
        super(surface);
        consumer = new PacketQueueConsumer(100);
        packetQueueConsumer = new Thread(consumer);
        decodedPacketConsumer = new Thread(new DecodedFrameConsumer(100));
    }

    @Override
    protected void onCreatedMediaCodec(MediaCodec mediaCodec) {}

    @Override
    protected void onStartedCodec(MediaCodec mediaCodec) {
        packetQueueConsumer.start();
        decodedPacketConsumer.start();
    }

    @Override
    protected void onReceiveH264Packet(H264Packet packet) {
        consumer.queueEncodedPacket(packet);
    }

    @Override
    public void close() throws InterruptedException {
        super.close();
    }

    /**
     * Consumes the FIFO (first-in-first-out) queue of packets being submitted to the decoder and
     * writes them to the {@link MediaCodec} input buffers.
     */
    private class PacketQueueConsumer implements Runnable {

        private final int timeout;

        private LinkedBlockingDeque<H264Packet> packetQueue;

        public PacketQueueConsumer(int timeout) {
            this.timeout = timeout;
            packetQueue = new LinkedBlockingDeque<>();
        }

        protected void queueEncodedPacket(H264Packet packet) {
            packetQueue.add(packet);
            logger.debug("Queued packet, new queue length: {}", packetQueue.size());
        }

        @Override
        public void run() {
            H264Packet packet;
            while (true) {
                packet = null;
                try {
                    packet = packetQueue.takeFirst();
                } catch (InterruptedException ex) {}

                if (packet != null) {
                    int bufferId = getMediaCodec().dequeueInputBuffer(timeout);
                    if (packet.segment == null) {
                        return;
                    }
                    writePacketToInputBuffer(packet, bufferId);
                    logger.debug("Submitting to decoder: {}", packet.toString());
                    int flags = decoderFlagsForPacket(packet);
                    getMediaCodec().queueInputBuffer(
                            bufferId, 0, packet.segment.getBufferSize(), packet.presentationTime, flags);
                }
            }
        }

    }

    private class DecodedFrameConsumer implements Runnable {

        private final int timeout;

        public DecodedFrameConsumer(int timeout) {
            this.timeout = timeout;
        }

        @Override
        public void run() {
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            while (getMediaCodec() != null && true) {
                int bufferId = getMediaCodec().dequeueOutputBuffer(bufferInfo, timeout);
                if (bufferId >= 0) {
                    logger.debug("DecodedFrameConsumer got buffer ID: {}", bufferId);
                    getMediaCodec().releaseOutputBuffer(bufferId, bufferInfo.size > 0);
                } else if (bufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    logger.debug("DecodedFrameConsumer got buffer ID: {}", bufferId);
                    setMediaFormat(getMediaCodec().getOutputFormat());
                }
            }
        }

    }

}
