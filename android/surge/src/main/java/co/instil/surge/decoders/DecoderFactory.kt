/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders

import co.instil.surge.client.SurgeVideoView
import co.instil.surge.decoders.mjpeg.MjpegDecoder
import co.instil.surge.decoders.mp4v.AsyncMp4vDecoder
import co.instil.surge.decoders.mpeg.h264.H264Decoder
import co.instil.surge.decoders.mpeg.h265.H265Decoder
import co.instil.surge.diagnostics.DiagnosticsTracker
import co.instil.surge.logging.LoggerFactory

/**
 * Factory to generate each supported type of decoder.
 */
object DecoderFactory {
    private val LOGGER = LoggerFactory.getLogger(DecoderFactory::class.java)

    @JvmStatic
    fun generateH264Decoder(videoView: SurgeVideoView, diagnostics: DiagnosticsTracker) =
        H264Decoder(videoView, diagnostics)

    @JvmStatic
    fun generateH265Decoder(videoView: SurgeVideoView, diagnostics: DiagnosticsTracker) =
        H265Decoder(videoView, diagnostics)

    @JvmStatic
    fun generateMP4VDecoder(videoView: SurgeVideoView, diagnostics: DiagnosticsTracker) =
        AsyncMp4vDecoder(videoView, diagnostics)

    @JvmStatic
    fun generateMJPEGDecoder(videoView: SurgeVideoView, diagnostics: DiagnosticsTracker) =
        MjpegDecoder(videoView, diagnostics)
}
