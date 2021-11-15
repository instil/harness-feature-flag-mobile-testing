/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg.h265

import android.media.MediaCodec
import co.instil.surge.client.SessionType
import co.instil.surge.client.SurgeVideoView
import co.instil.surge.decoders.MediaCodecFactory
import co.instil.surge.decoders.mpeg.MpegDecoder
import co.instil.surge.decoders.mpeg.NaluSegment
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluParser
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluSegment
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.PPS
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.SPS
import co.instil.surge.decoders.mpeg.h265.nalu.H265NaluType.VPS
import co.instil.surge.diagnostics.DiagnosticsTracker
import co.instil.surge.logging.LoggerFactory

class H265Decoder(
    private val videoView: SurgeVideoView,
    private val mediaCodecFactory: MediaCodecFactory,
    naluParser: H265NaluParser,
    diagnosticsTracker: DiagnosticsTracker
) : MpegDecoder(videoView, mediaCodecFactory, naluParser, diagnosticsTracker) {
    override val logger = LoggerFactory.getLogger(H265Decoder::class.java)

    private var videoParameterSet: H265NaluSegment? = null
    private var pictureParameterSet: H265NaluSegment? = null
    private var sequenceParameterSet: H265NaluSegment? = null
    private var cachedSegments: MutableList<H265NaluSegment> = mutableListOf()

    constructor(videoView: SurgeVideoView, diagnosticsTracker: DiagnosticsTracker) :
        this(
            videoView,
            MediaCodecFactory(),
            H265NaluParser(),
            diagnosticsTracker
        )

    override fun decodeNaluSegments(segments: List<NaluSegment>, presentationTime: Int) {
        decodeParameterSetNaluSegments(segments.filterIsInstance<H265NaluSegment>(), presentationTime)
        decodeNonParameterSetNaluSegments(segments.filterIsInstance<H265NaluSegment>(), presentationTime)
    }

    private fun decodeParameterSetNaluSegments(segments: List<H265NaluSegment>, presentationTime: Int) {
        segments.firstOrNull { it.type == VPS }?.let { onReceiveMpegPacket(H265Packet(it, presentationTime.toLong())) }
        segments.firstOrNull { it.type == SPS }?.let { onReceiveMpegPacket(H265Packet(it, presentationTime.toLong())) }
        segments.firstOrNull { it.type == PPS }?.let { onReceiveMpegPacket(H265Packet(it, presentationTime.toLong())) }
    }

    private fun decodeNonParameterSetNaluSegments(segments: List<H265NaluSegment>, presentationTime: Int) {
        cachedSegments = segments.filter { it.type != VPS && it.type != SPS && it.type != PPS }.toMutableList()
        val packet = buildH265FrameFromNaluSegments(presentationTime)
        onReceiveMpegPacket(packet)
        cachedSegments.clear()
    }

    private fun buildH265FrameFromNaluSegments(presentationTime: Int): H265Packet {
        val type = cachedSegments[0].type
        val magicHeaderSize = 3
        val bytes = ByteArray(cachedSegments.sumOf(H265NaluSegment::payloadLength) - magicHeaderSize)
        var startPosition = 0
        for (i in cachedSegments.indices) {
            var length = cachedSegments[i].payloadLength
            var sourcePosition = 0
            if (i == 0) {
                length -= magicHeaderSize
                sourcePosition = magicHeaderSize
            }
            System.arraycopy(cachedSegments[i].payload, sourcePosition, bytes, startPosition, length)
            startPosition += length
        }
        val naluSegment = H265NaluSegment(type, bytes)
        return H265Packet(naluSegment, presentationTime.toLong())
    }

    override fun hasCachedParameterSets() =
        videoParameterSet != null &&
            sequenceParameterSet != null &&
            pictureParameterSet != null

    override fun containsParameterSets(segments: List<NaluSegment>) =
        segments.any { it.type == VPS } &&
            segments.any { it.type == SPS } &&
            segments.any { it.type == PPS }

    override fun cacheParameterSets(segments: List<NaluSegment>) {
        segments.firstOrNull { it.type == VPS }
            .let { videoParameterSet = it as H265NaluSegment }
        segments.firstOrNull { it.type == SPS }
            .let { sequenceParameterSet = it as H265NaluSegment }
        segments.firstOrNull { it.type == PPS }
            .let { pictureParameterSet = it as H265NaluSegment }
    }

    override fun trackDiagnostics() {
        diagnosticsTracker.trackNewMediaFormat(SessionType.H265)
    }

    override fun createMediaCodec(): MediaCodec =
            mediaCodecFactory.createH265DecoderWithParameters(
                videoParameterSet as H265NaluSegment,
                sequenceParameterSet as H265NaluSegment,
                pictureParameterSet as H265NaluSegment,
                videoView.generateUniqueSurface()
            )
}
