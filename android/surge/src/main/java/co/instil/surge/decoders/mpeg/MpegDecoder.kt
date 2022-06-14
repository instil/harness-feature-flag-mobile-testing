/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mpeg

import android.media.MediaCodec
import android.media.MediaFormat
import android.os.Handler
import android.os.Looper
import co.instil.surge.client.SessionDescription
import co.instil.surge.client.SurgeVideoView
import co.instil.surge.decoders.Decoder
import co.instil.surge.decoders.MediaCodecFactory
import co.instil.surge.diagnostics.DiagnosticsTracker
import co.instil.surge.logging.Logger
import java.io.IOException
import java.nio.ByteBuffer
import java.util.ArrayList

abstract class MpegDecoder(
        private val videoView: SurgeVideoView,
        private val mediaCodecFactory: MediaCodecFactory,
        private val naluParser: NaluParser,
        internal val diagnosticsTracker: DiagnosticsTracker
        ) : Decoder {
    abstract val logger: Logger

    private var mediaCodec: MediaCodec? = null
    private var mediaFormat: MediaFormat? = null
    protected val availableInputBuffers: MutableList<Int> = ArrayList()
    protected val decodeQueue: MutableList<MpegPacket> = ArrayList()

    @Throws(InterruptedException::class)
    override fun close() {
        Handler(Looper.getMainLooper()).post {
            try {
                mediaCodec?.stop()
                mediaCodec?.release()
            } catch (e: java.lang.Exception) {
                logger.error("Failed to close MediaCodec", e)
            }
        }
    }

    override fun decodeFrameBuffer(
        sessionDescription: SessionDescription,
        frameBuffer: ByteBuffer,
        width: Int,
        height: Int,
        presentationTime: Int,
        duration: Int
    ) {
        logger.debug("Received frame buffer for decoding")
        diagnosticsTracker.trackNewFrameOfSize(frameBuffer.remaining() * 8)

        try {
            val segments: List<NaluSegment> = naluParser.parseNaluSegments(frameBuffer)

            if (!hasCachedParameterSets() && mediaCodec == null && !containsParameterSets(segments)) {
                return
            }
            startMediaCodec(segments)
            decodeNaluSegments(segments, presentationTime)
        } catch (e: Exception) {
            logger.error("Failed to decode frame", e)
        }
    }

    abstract fun decodeNaluSegments(segments: List<NaluSegment>, presentationTime: Int)

    private fun startMediaCodec(segments: List<NaluSegment>) {
        if (!hasCachedParameterSets() && mediaCodec == null) {
            logger.debug("Decoder received parameter sets")
            cacheParameterSets(segments)
            mediaCodec = createMediaCodec()
            onCreatedMediaCodec(mediaCodec)
            mediaCodec?.start()
        }
    }

    protected open fun onCreatedMediaCodec(mediaCodec: MediaCodec?) {
        mediaCodec?.setCallback(MpegDecoderCallbackHandler())
    }

    protected open fun onReceiveMpegPacket(packet: MpegPacket) {
        if (availableInputBuffers.isNotEmpty()) {
            logger.debug("Submitting to available buffer: {}", availableInputBuffers[0])
            submitToDecoder(packet, availableInputBuffers.removeAt(0), mediaCodec)
        } else {
            decodeQueue.add(packet)
        }
    }

    protected fun submitToDecoder(packet: MpegPacket, bufferIndex: Int, codec: MediaCodec?) {
        logger.debug("Decode Queue Size: {}", decodeQueue.size)
        writePacketToInputBuffer(packet, bufferIndex)
        logger.debug("Submitting to decoder: {}", packet.toString())
        val flags = decoderFlagsForPacket(packet)
        codec?.queueInputBuffer(bufferIndex, 0, packet.segment.payloadLength, packet.presentationTime, flags)
    }

    fun writePacketToInputBuffer(packet: MpegPacket?, inputBufferId: Int) {
        if (inputBufferId == -1 || packet == null) {
            return
        }

        val buffer = mediaCodec?.getInputBuffer(inputBufferId)
        buffer?.clear()
        val payload = packet.segment.payload
        if (payload.isNotEmpty()) {
            buffer?.put(payload, 0, payload.size)
        }
    }

    private fun decoderFlagsForPacket(packet: MpegPacket): Int {
        var flags = packet.flags
        if (packet.isKeyFrame) {
            flags = flags and MediaCodec.BUFFER_FLAG_KEY_FRAME
        }
        if (packet.isParameterSet) {
            flags = flags and MediaCodec.BUFFER_FLAG_CODEC_CONFIG
        }
        return flags
    }

    @Throws(IOException::class)
    protected abstract fun createMediaCodec(): MediaCodec

    /**
     * Determines whether the decoder has yet encountered and stored Paramater Set NAL units.
     * @return true if the decoder has been initialised with Paramater Set.
     */
    protected abstract fun hasCachedParameterSets(): Boolean

    /**
     * Determines whether the list of [NaluSegment] contains the
     * NAL units which are used to configure the decoder.
     * @param segments the segments to be inspected for data.
     * @return true if the list contains Parameter Set NAL units; false otherwise.
     */
    protected abstract fun containsParameterSets(segments: List<NaluSegment>): Boolean

    protected abstract fun cacheParameterSets(segments: List<NaluSegment>)

    private fun setMediaFormat(mediaFormat: MediaFormat) {
        this.mediaFormat = mediaFormat
        if (mediaFormatHasStreamDimensions(mediaFormat)) {
            val streamWidth = mediaFormat.getInteger(MEDIA_FORMAT_WIDTH_KEY)
            val streamHeight = mediaFormat.getInteger(MEDIA_FORMAT_HEIGHT_KEY)
            videoView.setVideoDimensions(streamWidth, streamHeight)
            trackDiagnostics(streamWidth, streamHeight)
        }
    }

    private fun trackDiagnostics(streamWidth: Int, streamHeight: Int) {
        diagnosticsTracker.trackNewFrameDimensions(streamWidth, streamHeight)
        trackDiagnostics()
    }

    protected abstract fun trackDiagnostics()

    private fun mediaFormatHasStreamDimensions(mediaFormat: MediaFormat): Boolean {
        return mediaFormat.containsKey(MEDIA_FORMAT_WIDTH_KEY) && mediaFormat.containsKey(MEDIA_FORMAT_HEIGHT_KEY)
    }

    private inner class MpegDecoderCallbackHandler : MediaCodec.Callback() {
        override fun onInputBufferAvailable(codec: MediaCodec, index: Int) {
            if (decodeQueue.isEmpty()) {
                availableInputBuffers.add(index)
                return
            }
            val packet = decodeQueue.removeAt(0)
            try {
                submitToDecoder(packet, index, codec)
            } catch (e: Exception) {
                logger.error("Failed to decode frame: $e")
                logger.printStackTrace(e)
            }
        }

        override fun onOutputBufferAvailable(codec: MediaCodec, index: Int, info: MediaCodec.BufferInfo) {
            try {
                codec.releaseOutputBuffer(index, info.size > 0)
            } catch (e: Exception) {
                logger.error("Failed to release output buffer: $e")
                logger.printStackTrace(e)
            }
        }

        override fun onError(codec: MediaCodec, exception: MediaCodec.CodecException) {
            logger.error("Decoder callback error: $exception")
        }

        override fun onOutputFormatChanged(codec: MediaCodec, format: MediaFormat) {
            setMediaFormat(format)
        }
    }

    companion object {
        internal const val MEDIA_FORMAT_WIDTH_KEY = "width"
        internal const val MEDIA_FORMAT_HEIGHT_KEY = "height"
    }
}
