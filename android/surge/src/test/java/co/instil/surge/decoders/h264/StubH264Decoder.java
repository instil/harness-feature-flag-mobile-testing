/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.h264;

import android.media.MediaCodec;
import android.view.Surface;
import co.instil.surge.decoders.MediaCodecFactory;
import co.instil.surge.decoders.h264.nalu.NaluParser;
import co.instil.surge.device.DeviceExaminer;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by mattmccomb on 08/07/16.
 */
public class StubH264Decoder extends H264Decoder {

    private List<H264Packet> receivedPackets;

    protected StubH264Decoder(Surface surface) {
        super(surface);
        receivedPackets = new ArrayList<>();
    }

    protected StubH264Decoder(
            Surface surface,
            MediaCodecFactory mediaCodecFactory,
            NaluParser naluParser,
            DeviceExaminer deviceExaminer) {
        super(surface, mediaCodecFactory, naluParser, deviceExaminer);
        receivedPackets = new ArrayList<>();
    }

    @Override
    protected void onCreatedMediaCodec(MediaCodec mediaCodec) {

    }

    @Override
    protected void onStartedCodec(MediaCodec mediaCodec) {

    }

    @Override
    protected void onReceiveH264Packet(H264Packet packet) {
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
