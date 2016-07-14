package co.instil.surge.decoders.h264;

import android.media.MediaCodec;
import android.view.Surface;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by mattmccomb on 08/07/16.
 */
public class StubH264Decoder extends H264Decoder {

    public MediaCodec mediaCodec;

    private List<H264Packet> receivedPackets;

    protected StubH264Decoder(Surface surface) {
        super(surface);
        receivedPackets = new ArrayList<>();
    }

    @Override
    protected void onCreatedMediaCodec(MediaCodec mediaCodec) {
        this.mediaCodec = mediaCodec;
    }

    @Override
    protected void onStartedCodec(MediaCodec mediaCodec) {

    }

    @Override
    protected void onReceiveH264Packet(H264Decoder.H264Packet packet) {
        receivedPackets.add(packet);
    }

    protected int numberOfReceivedPackets() {
        return receivedPackets.size();
    }

    protected H264Packet lastReceivedPacket() {
        if (numberOfReceivedPackets() == 0) {
            return null;
        }
        return receivedPackets.get(receivedPackets.size() - 1);
    }

}
