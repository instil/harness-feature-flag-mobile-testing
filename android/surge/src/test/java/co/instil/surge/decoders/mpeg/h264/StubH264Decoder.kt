/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.mpeg.h264

import android.media.MediaCodec
import co.instil.surge.client.SurgeVideoView
import co.instil.surge.decoders.MediaCodecFactory
import co.instil.surge.decoders.mpeg.MpegPacket
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluParser
import co.instil.surge.diagnostics.DiagnosticsTracker
import java.util.ArrayList

/**
 * Created by mattmccomb on 08/07/16.
 */
class StubH264Decoder : H264Decoder {
    private var receivedPackets: MutableList<H264Packet>

    constructor(
        videoView: SurgeVideoView,
        diagnosticsTracker: DiagnosticsTracker
    ) : super(videoView, diagnosticsTracker) {
        receivedPackets = ArrayList()
    }

    constructor(
        videoView: SurgeVideoView,
        mediaCodecFactory: MediaCodecFactory,
        naluParser: H264NaluParser,
        diagnosticsTracker: DiagnosticsTracker
    ) : super(videoView, mediaCodecFactory, naluParser, diagnosticsTracker) {
        receivedPackets = ArrayList()
    }

    override fun onCreatedMediaCodec(mediaCodec: MediaCodec?) {
        // do nothing
    }

    override fun onReceiveMpegPacket(packet: MpegPacket) {
        receivedPackets.add(packet as H264Packet)
    }

    fun numberOfReceivedPackets(): Int = receivedPackets.size
}