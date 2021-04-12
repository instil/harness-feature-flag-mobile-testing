package co.instil.surge.decoders.h265

import android.media.MediaCodec
import android.media.MediaCodec.BUFFER_FLAG_CODEC_CONFIG
import android.media.MediaCodec.BUFFER_FLAG_KEY_FRAME
import android.media.MediaFormat
import android.os.Handler
import android.os.Looper
import co.instil.surge.client.SessionDescription
import co.instil.surge.client.SurgeVideoView
import co.instil.surge.decoders.Decoder
import co.instil.surge.decoders.MediaCodecFactory
import co.instil.surge.decoders.h265.nalu.H265NaluParser
import co.instil.surge.decoders.h265.nalu.H265NaluSegment
import co.instil.surge.decoders.h265.nalu.H265NaluType.PPS
import co.instil.surge.decoders.h265.nalu.H265NaluType.SPS
import co.instil.surge.decoders.h265.nalu.H265NaluType.VPS
import co.instil.surge.diagnostics.DiagnosticsTracker
import co.instil.surge.logging.LoggerFactory
import java.nio.ByteBuffer
import java.util.ArrayList

class H265Decoder(
    private val videoView: SurgeVideoView,
    private val mediaCodecFactory: MediaCodecFactory,
    private val naluParser: H265NaluParser,
    private val diagnosticsTracker: DiagnosticsTracker
) : Decoder {

    private var mediaCodec: MediaCodec? = null
    private lateinit var mediaFormat: MediaFormat
    private var cachedSegments: MutableList<H265NaluSegment> = mutableListOf()
    private var pictureParameterSet: H265NaluSegment? = null
    private var sequenceParameterSet: H265NaluSegment? = null
    private var videoParameterSet: H265NaluSegment? = null
    private val availableInputBuffers: MutableList<Int> = ArrayList()
    private val decodeQueue: MutableList<H265Packet> = ArrayList()

    constructor(videoView: SurgeVideoView, diagnosticsTracker: DiagnosticsTracker) :
        this(
            videoView,
            MediaCodecFactory(),
            H265NaluParser(),
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
            val segments: List<H265NaluSegment> = naluParser.parseH265NaluSegments(frameBuffer)

            if (!hasCachedParameterSets() && mediaCodec == null && !containsParameterSets(segments)) {
                return
            }
            startMediaCodec(segments)
            decodeParameterSetNaluSegments(segments, presentationTime)
            decodeNonParameterSetNaluSegments(segments, presentationTime)
        } catch (e: Exception) {
            LOGGER.error("Failed to decode frame", e)
        }
    }

    private fun decodeParameterSetNaluSegments(segments: List<H265NaluSegment>, presentationTime: Int) {
        segments.firstOrNull { it.type == VPS }?.let { onReceiveH265Packet(H265Packet(it, presentationTime.toLong())) }
        segments.firstOrNull { it.type == SPS }?.let { onReceiveH265Packet(H265Packet(it, presentationTime.toLong())) }
        segments.firstOrNull { it.type == PPS }?.let { onReceiveH265Packet(H265Packet(it, presentationTime.toLong())) }
    }

    private fun decodeNonParameterSetNaluSegments(segments: List<H265NaluSegment>, presentationTime: Int) {
        cachedSegments = segments.filter { it.type != VPS && it.type != SPS && it.type != PPS }.toMutableList()
        val packet = buildH265FrameFromNaluSegments(presentationTime)
        onReceiveH265Packet(packet)
        cachedSegments.clear()
    }

    private fun startMediaCodec(segments: List<H265NaluSegment>) {
        if (!hasCachedParameterSets() && mediaCodec == null) {
            LOGGER.debug("Decoder received parameter sets")
            cacheParameterSets(segments)
            mediaCodec = createMediaCodec()
            onCreatedMediaCodec(mediaCodec)
            mediaCodec?.start()
        }
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

    private fun onCreatedMediaCodec(mediaCodec: MediaCodec?) {
        mediaCodec?.setCallback(H265DecoderCallbackHandler())
    }

    private fun onReceiveH265Packet(packet: H265Packet) {
        if (availableInputBuffers.isNotEmpty()) {
            LOGGER.debug("Submitting to available buffer: {}", availableInputBuffers[0])
            submitToDecoder(packet, availableInputBuffers.removeAt(0), mediaCodec)
        } else {
            decodeQueue.add(packet)
        }
    }

    private fun containsParameterSets(segments: List<H265NaluSegment>) =
        segments.any { it.type == VPS } &&
            segments.any { it.type == SPS } &&
            segments.any { it.type == PPS }

    private fun hasCachedParameterSets() =
        videoParameterSet != null &&
            sequenceParameterSet != null &&
            pictureParameterSet != null

    private fun cacheParameterSets(segments: List<H265NaluSegment>) {
        segments.firstOrNull { it.type == VPS }
            .let { videoParameterSet = it }
        segments.firstOrNull { it.type == SPS }
            .let { sequenceParameterSet = it }
        segments.firstOrNull { it.type == PPS }
            .let { pictureParameterSet = it }
    }

    private fun submitToDecoder(packet: H265Packet, bufferIndex: Int, codec: MediaCodec?) {
        LOGGER.debug("Decode Queue Size: {}", decodeQueue.size)
        writePacketToInputBuffer(packet, bufferIndex)
        LOGGER.debug("Submitting to decoder: {}", packet.toString())
        val flags = decoderFlagsForPacket(packet)
        codec?.queueInputBuffer(bufferIndex, 0, packet.naluSegment.payloadLength, packet.presentationTime, flags)
    }

    private fun writePacketToInputBuffer(packet: H265Packet?, inputBufferId: Int) {
        if (inputBufferId == -1 || packet == null) {
            return
        }

        val buffer = mediaCodec?.getInputBuffer(inputBufferId)
        buffer?.clear()
        val payload = packet.naluSegment.payload
        if (payload.isNotEmpty()) {
            buffer?.put(payload, 0, payload.size)
        }
    }

    private fun decoderFlagsForPacket(packet: H265Packet): Int {
        var flags = packet.flags
        if (packet.isKeyFrame) {
            flags += BUFFER_FLAG_KEY_FRAME
        }
        if (packet.isPictureParameterSet || packet.isSequenceParameterSet || packet.isVideoParameterSet) {
            flags += BUFFER_FLAG_CODEC_CONFIG
        }
        return flags
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

    private fun createMediaCodec() =
        mediaCodecFactory.createH265DecoderWithParameters(
            videoParameterSet,
            sequenceParameterSet,
            pictureParameterSet,
            videoView.generateUniqueSurface()
        )

    private inner class H265DecoderCallbackHandler : MediaCodec.Callback() {
        override fun onInputBufferAvailable(codec: MediaCodec, index: Int) {
            if (decodeQueue.isEmpty()) {
                availableInputBuffers.add(index)
                return
            }
            val packet: H265Packet = decodeQueue.removeAt(0)
            try {
                submitToDecoder(packet, index, codec)
            } catch (e: Exception) {
                LOGGER.error("Failed to decode H265 frame: $e")
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

        override fun onError(codec: MediaCodec, exception: MediaCodec.CodecException) {
            LOGGER.error("H265 decoder callback error: $exception")
        }

        override fun onOutputFormatChanged(codec: MediaCodec, format: MediaFormat) {
            setMediaFormat(format)
        }
    }

    companion object {
        private val LOGGER = LoggerFactory.getLogger(H265Decoder::class.java)
        private const val MEDIA_FORMAT_WIDTH_KEY = "width"
        private const val MEDIA_FORMAT_HEIGHT_KEY = "height"
    }
}
