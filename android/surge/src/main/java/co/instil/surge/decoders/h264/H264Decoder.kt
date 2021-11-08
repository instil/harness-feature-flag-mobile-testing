/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.h264

import android.media.MediaCodec
import android.media.MediaCodec.CodecException
import android.media.MediaFormat
import android.os.Handler
import android.os.Looper
import co.instil.surge.client.SessionDescription
import co.instil.surge.client.SurgeVideoView
import co.instil.surge.decoders.Decoder
import co.instil.surge.decoders.MediaCodecFactory
import co.instil.surge.decoders.h264.nalu.H264NaluParser
import co.instil.surge.decoders.h264.nalu.H264NaluSegment
import co.instil.surge.decoders.h264.nalu.H264NaluType
import co.instil.surge.diagnostics.DiagnosticsTracker
import co.instil.surge.logging.LoggerFactory
import java.io.IOException
import java.lang.Exception
import java.nio.ByteBuffer
import java.util.ArrayList
import kotlin.Throws

/**
 * An H264 stream decoder which utilises native hardware decoders and the
 * [MediaCodec] Android platform APIs.
 *
 * @see MediaCodec
 */
open class H264Decoder protected constructor(
    private val videoView: SurgeVideoView,
    private val mediaCodecFactory: MediaCodecFactory,
    private val naluParser: H264NaluParser,
    private val diagnosticsTracker: DiagnosticsTracker
) : Decoder {

    private var mediaCodec: MediaCodec? = null
    private var mediaFormat: MediaFormat? = null
    private var pictureParameterSet: H264NaluSegment? = null
    private var sequenceParameterSet: H264NaluSegment? = null
    private var lastSPSPacket: H264Packet? = null
    private var lastPPSPacket: H264Packet? = null
    private val availableInputBuffers: MutableList<Int> = ArrayList()
    private val decodeQueue: MutableList<H264Packet> = ArrayList()

    constructor(videoView: SurgeVideoView, diagnosticsTracker: DiagnosticsTracker) :
        this(
            videoView,
            MediaCodecFactory(),
            H264NaluParser(),
            diagnosticsTracker
        )

    override fun decodeFrameBuffer(
        sessionDescription: SessionDescription,
        frameBuffer: ByteBuffer,
        width: Int,
        height: Int,
        presentationTime: Int,
        duration: Int
    ) {
        LOGGER.debug("Received frame buffer for decoding")
        diagnosticsTracker.trackNewFrameOfSize(frameBuffer.remaining() * 8)

        try {
            val segments = naluParser.parseNaluSegments(frameBuffer)

            if (!hasCachedParameterSets() && mediaCodec == null && !containsParameterSets(segments)) {
                return
            }
            startMediaCodec(segments)
            for (unit in segments) {
                val packet = H264Packet(unit, presentationTime.toLong())
                processH264Packet(packet)
            }
        } catch (ex: Exception) {
            LOGGER.error("Failed to decode frame", ex)
        }
    }

    private fun startMediaCodec(segments: List<H264NaluSegment>) {
        if (!hasCachedParameterSets() && mediaCodec == null) {
            LOGGER.debug("Decoder received parameter sets")
            cacheParameterSets(segments)
            mediaCodec = createMediaCodec()
            onCreatedMediaCodec(mediaCodec)
            mediaCodec?.start()
        }
    }

    private fun processH264Packet(packet: H264Packet) = when {
        packet.isKeyFrame -> {
            lastSPSPacket?.let { onReceiveH264Packet(it) }
            lastPPSPacket?.let { onReceiveH264Packet(it) }
            onReceiveH264Packet(packet)
        }
        packet.isSequenceParameterSet -> lastSPSPacket = packet
        packet.isPictureParameterSet -> lastPPSPacket = packet
        else -> onReceiveH264Packet(packet)
    }

    @Throws(InterruptedException::class)
    override fun close() {
        Handler(Looper.getMainLooper()).post {
            try {
                mediaCodec?.stop()
                mediaCodec?.release()
            } catch (e: Exception) {
                LOGGER.error("Failed to close MediaCodec", e)
            }
        }
    }

    protected open fun onCreatedMediaCodec(mediaCodec: MediaCodec?) {
        mediaCodec?.setCallback(H264DecoderCallbackHandler())
    }

    protected open fun onStartedCodec(mediaCodec: MediaCodec?) {}

    protected open fun onReceiveH264Packet(packet: H264Packet) {
        if (!availableInputBuffers.isEmpty()) {
            LOGGER.debug("Submitting to available buffer: {}", availableInputBuffers[0])
            submitToDecoder(packet, availableInputBuffers.removeAt(0), mediaCodec)
        } else {
            decodeQueue.add(packet)
        }
    }

    private fun submitToDecoder(packet: H264Packet, bufferIndex: Int, codec: MediaCodec?) {
        LOGGER.debug("Decode Queue Size: {}", decodeQueue.size)
        writePacketToInputBuffer(packet, bufferIndex)
        LOGGER.debug("Submitting to decoder: {}", packet.toString())
        val flags = decoderFlagsForPacket(packet)
        codec?.queueInputBuffer(bufferIndex, 0, packet.segment.payloadLength, packet.presentationTime, flags)
    }

    /**
     * Determines whether the list of [H264NaluSegment] contains the 'Sequence Paramater Set'
     * (SPS) and 'Picture Parameter Set' (PPS) NAL units; which are used to configure the decoder.
     * @param segments the segments to be inspected for SPS / PPS data.
     * @return true if the list contains SPS and PPS NAL units; false otherwise.
     */
    private fun containsParameterSets(segments: List<H264NaluSegment>): Boolean {
        var containsSps = false
        var containsPps = false
        for (segment in segments) {
            if (segment.type == H264NaluType.SPS) {
                containsSps = true
            } else if (segment.type == H264NaluType.PPS) {
                containsPps = true
            }
        }
        return containsSps && containsPps
    }

    /**
     * Determines whether the decoder has yet encountered and stored 'Sequence Paramater Set'
     * (SPS) and 'Picture Parameter Set' (PPS) NAL units.
     * @return true if the decoder has been initialised with SPS and PSS.
     */
    private fun hasCachedParameterSets(): Boolean {
        return pictureParameterSet != null && sequenceParameterSet != null
    }

    private fun cacheParameterSets(segments: List<H264NaluSegment>) {
        for (segment in segments) {
            if (segment.type == H264NaluType.SPS) {
                setSequenceParameterSet(segment)
            } else if (segment.type == H264NaluType.PPS) {
                setPictureParameterSet(segment)
            }
        }
    }

    fun writePacketToInputBuffer(packet: H264Packet?, inputBufferId: Int) {
        if (inputBufferId == -1 || packet == null) {
            return
        }
        val buffer: ByteBuffer? = mediaCodec?.getInputBuffer(inputBufferId)
        buffer?.clear()
        val payload = packet.segment.payload
        if (payload.isNotEmpty()) {
            buffer?.put(payload, 0, payload.size)
        }
    }

    private fun setPictureParameterSet(segment: H264NaluSegment) {
        pictureParameterSet = segment
    }

    private fun decoderFlagsForPacket(packet: H264Packet): Int {
        var flags = 0
        if (packet.isKeyFrame) {
            flags = flags and MediaCodec.BUFFER_FLAG_KEY_FRAME
        }
        if (packet.isPictureParameterSet || packet.isSequenceParameterSet) {
            flags = flags and MediaCodec.BUFFER_FLAG_CODEC_CONFIG
        }
        return flags
    }

    private fun setSequenceParameterSet(segment: H264NaluSegment) {
        sequenceParameterSet = segment
    }

    private fun setMediaFormat(mediaFormat: MediaFormat) {
        this.mediaFormat = mediaFormat
        if (mediaFormatHasStreamDimensions(mediaFormat)) {
            val streamWidth = mediaFormat.getInteger(MEDIA_FORMAT_WIDTH_KEY)
            val streamHeight = mediaFormat.getInteger(MEDIA_FORMAT_HEIGHT_KEY)
            videoView.setVideoDimensions(streamWidth, streamHeight)
            diagnosticsTracker.trackNewFrameDimensions(streamWidth, streamHeight)
        }
    }

    private fun mediaFormatHasStreamDimensions(mediaFormat: MediaFormat): Boolean {
        return mediaFormat.containsKey(MEDIA_FORMAT_WIDTH_KEY) && mediaFormat.containsKey(MEDIA_FORMAT_HEIGHT_KEY)
    }

    @Throws(IOException::class)
    private fun createMediaCodec(): MediaCodec {
        return mediaCodecFactory.createH264DecoderWithParameters(
            sequenceParameterSet,
            pictureParameterSet,
            videoView.generateUniqueSurface()
        )
    }

    private inner class H264DecoderCallbackHandler : MediaCodec.Callback() {
        override fun onInputBufferAvailable(codec: MediaCodec, index: Int) {
            if (decodeQueue.isEmpty()) {
                availableInputBuffers.add(index)
                return
            }
            val packet = decodeQueue.removeAt(0)
            try {
                submitToDecoder(packet, index, codec)
            } catch (e: Exception) {
                LOGGER.error("Failed to decode H264 frame: $e")
                LOGGER.printStackTrace(e)
            }
        }

        override fun onOutputBufferAvailable(codec: MediaCodec, index: Int, info: MediaCodec.BufferInfo) {
            try {
                codec.releaseOutputBuffer(index, info.size > 0)
            } catch (e: Exception) {
                LOGGER.error("Failed to release output buffer: $e")
                LOGGER.printStackTrace(e)
            }
        }

        override fun onError(codec: MediaCodec, exception: CodecException) {
            LOGGER.error("H264 decoder callback error: $exception")
        }

        override fun onOutputFormatChanged(codec: MediaCodec, format: MediaFormat) {
            setMediaFormat(format)
        }
    }

    companion object {
        private val LOGGER = LoggerFactory.getLogger(H264Decoder::class.java)
        private const val MEDIA_FORMAT_WIDTH_KEY = "width"
        private const val MEDIA_FORMAT_HEIGHT_KEY = "height"
    }
}
