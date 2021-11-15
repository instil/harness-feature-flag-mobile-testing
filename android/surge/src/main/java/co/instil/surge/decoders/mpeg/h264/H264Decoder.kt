/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.mpeg.h264

import android.media.MediaCodec
import co.instil.surge.client.SessionType
import co.instil.surge.client.SurgeVideoView
import co.instil.surge.decoders.MediaCodecFactory
import co.instil.surge.decoders.mpeg.MpegDecoder
import co.instil.surge.decoders.mpeg.NaluSegment
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluParser
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluSegment
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluType.PPS
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluType.SPS
import co.instil.surge.diagnostics.DiagnosticsTracker
import co.instil.surge.logging.LoggerFactory

open class H264Decoder(
    private val videoView: SurgeVideoView,
    private val mediaCodecFactory: MediaCodecFactory,
    naluParser: H264NaluParser,
    diagnosticsTracker: DiagnosticsTracker
) : MpegDecoder(videoView, mediaCodecFactory, naluParser, diagnosticsTracker) {
    override val logger = LoggerFactory.getLogger(H264Decoder::class.java)

    private var pictureParameterSet: H264NaluSegment? = null
    private var sequenceParameterSet: H264NaluSegment? = null
    private var lastSPSPacket: H264Packet? = null
    private var lastPPSPacket: H264Packet? = null

    constructor(videoView: SurgeVideoView, diagnosticsTracker: DiagnosticsTracker) :
        this(
            videoView,
            MediaCodecFactory(),
            H264NaluParser(),
            diagnosticsTracker
        )

    override fun decodeNaluSegments(segments: List<NaluSegment>, presentationTime: Int) {
        segments.filterIsInstance<H264NaluSegment>()
            .map { H264Packet(it, presentationTime.toLong()) }
            .forEach { processH264Packet(it) }
    }

    private fun processH264Packet(packet: H264Packet) = when {
        packet.isKeyFrame -> {
            lastSPSPacket?.let { onReceiveMpegPacket(it) }
            lastPPSPacket?.let { onReceiveMpegPacket(it) }
            onReceiveMpegPacket(packet)
        }
        packet.segment.type == SPS -> lastSPSPacket = packet
        packet.segment.type == PPS -> lastPPSPacket = packet
        else -> onReceiveMpegPacket(packet)
    }

    override fun containsParameterSets(segments: List<NaluSegment>): Boolean =
        segments.any { it.type == SPS } && segments.any { it.type == PPS }

    override fun hasCachedParameterSets(): Boolean =
        pictureParameterSet != null && sequenceParameterSet != null

    override fun cacheParameterSets(segments: List<NaluSegment>) {
        segments.filterIsInstance<H264NaluSegment>().forEach {
            when (it.type) {
                SPS -> sequenceParameterSet = it
                PPS -> pictureParameterSet = it
                else -> {
                }
            }
        }
    }

    override fun trackDiagnostics() {
        diagnosticsTracker.trackNewMediaFormat(SessionType.H264)
    }

    override fun createMediaCodec(): MediaCodec =
            mediaCodecFactory.createH264DecoderWithParameters(
                sequenceParameterSet as H264NaluSegment,
                pictureParameterSet as H264NaluSegment,
                videoView.generateUniqueSurface()
            )
}
