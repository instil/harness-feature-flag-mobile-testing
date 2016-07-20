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
