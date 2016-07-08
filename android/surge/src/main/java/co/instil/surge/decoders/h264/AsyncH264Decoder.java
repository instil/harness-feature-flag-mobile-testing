package co.instil.surge.decoders.h264;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.List;

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

    private static final Logger logger = LoggerFactory.getLogger(AsyncH264Decoder.class);

    private List<Integer> availableInputBuffers = new ArrayList<>();
    private List<H264Packet> decodeQueue = new ArrayList<>();

    /**
     * Constrcuts a new {@link AsyncH264Decoder}.
     * @param surface the surface onto which the decoded stream will be rendered.
     */
    public AsyncH264Decoder(Surface surface) {
        super(surface);
    }

    @Override
    public void onCreatedMediaCodec(MediaCodec mediaCodec) {
        mediaCodec.setCallback(new AsyncH264DecoderCallbackHandler());
    }

    @Override
    protected void onStartedCodec(MediaCodec mediaCodec) {}

    @Override
    protected void onReceiveH264Packet(H264Packet packet) {
        if (availableInputBuffers.size() > 0) {
            logger.debug("Submitting to available buffer: {}", availableInputBuffers.get(0));
            submitToDecoder(packet, availableInputBuffers.remove(0), getMediaCodec());
        } else {
            decodeQueue.add(packet);
        }
    }

    @Override
    public void close() throws InterruptedException {
        super.close();
    }

    private void submitToDecoder(H264Packet packet, int bufferIndex, MediaCodec codec) {
        if (packet.segment == null) {
            return;
        }
        logger.debug("Decode Queue Size: {}", decodeQueue.size());
        writePacketToInputBuffer(packet, bufferIndex);
        logger.debug("Submitting to decoder: {}", packet.toString());
        int flags = decoderFlagsForPacket(packet);
        codec.queueInputBuffer(bufferIndex, 0, packet.segment.getBufferSize(), packet.presentationTime, flags);
    }

    private class AsyncH264DecoderCallbackHandler extends MediaCodec.Callback {

        @Override
        public void onInputBufferAvailable(MediaCodec codec, int index) {
            if (decodeQueue.size() == 0) {
                availableInputBuffers.add(index);
                return;
            }
            H264Packet packet = decodeQueue.remove(0);
            submitToDecoder(packet, index, codec);
        }

        @Override
        public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
            codec.releaseOutputBuffer(index, info.size > 0);
        }

        @Override
        public void onError(MediaCodec codec, MediaCodec.CodecException e) {
        }

        @Override
        public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
            setMediaFormat(format);
        }

    }



}