/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.mpeg.h264

import android.media.MediaCodec
import android.view.Surface
import co.instil.surge.client.SessionDescription
import co.instil.surge.client.SessionType
import co.instil.surge.client.SurgeVideoView
import co.instil.surge.decoders.MediaCodecFactory
import co.instil.surge.decoders.mpeg.h264.H264TestUtils.NaluSpec
import co.instil.surge.decoders.mpeg.h264.H264TestUtils.generateNalUnits
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluParser
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluSegment
import co.instil.surge.decoders.mpeg.h264.nalu.H264NaluType
import co.instil.surge.diagnostics.DiagnosticsTracker
import org.easymock.Capture
import org.easymock.EasyMock
import org.hamcrest.core.Is
import org.hamcrest.core.IsEqual
import org.junit.Assert
import org.junit.Before
import org.junit.Test
import java.io.IOException
import java.lang.Exception
import java.nio.ByteBuffer
import kotlin.Throws

/**
 * Created by mattmccomb on 08/07/16.
 */
class H264DecoderTest {
    private lateinit var surface: Surface
    private lateinit var videoView: SurgeVideoView
    private lateinit var stubbedDecoder: StubH264Decoder
    private lateinit var mockH264Codec: MediaCodec
    private lateinit var mockBuffer: ByteBuffer
    private lateinit var mockMediaCodecFactory: MediaCodecFactory
    private lateinit var mockedDecoder: StubH264Decoder
    private lateinit var diagnosticsTracker: DiagnosticsTracker
    private lateinit var capturedPPSSegment: Capture<H264NaluSegment>
    private lateinit var capturedSPSSegment: Capture<H264NaluSegment>

    @Before
    @Throws(Exception::class)
    fun setUp() {
        surface = EasyMock.createMock(Surface::class.java)
        videoView = EasyMock.createMock(SurgeVideoView::class.java)
        EasyMock.expect(videoView.generateUniqueSurface()).andReturn(surface)
        EasyMock.replay(videoView)
        diagnosticsTracker = DiagnosticsTracker()
        stubbedDecoder = StubH264Decoder(videoView, diagnosticsTracker)
        capturedSPSSegment = Capture.newInstance()
        capturedPPSSegment = Capture.newInstance()
        mockBuffer = EasyMock.createMock(ByteBuffer::class.java)
        mockH264Codec = EasyMock.createMock(MediaCodec::class.java)
        mockMediaCodecFactory = EasyMock.createMock(MediaCodecFactory::class.java)
        mockedDecoder = StubH264Decoder(videoView, mockMediaCodecFactory, H264NaluParser(), diagnosticsTracker)
        EasyMock.expect(
            mockMediaCodecFactory.createH264DecoderWithParameters(
                EasyMock.capture(capturedSPSSegment),
                EasyMock.capture(capturedPPSSegment),
                EasyMock.eq(surface)
            )
        ).andReturn(mockH264Codec)
        EasyMock.replay(mockMediaCodecFactory)
    }

    @Test
    fun testThatTheDecoderIgnoresFramesUntilItReceivesParameterSets() {
        sendKeyFrameToDecoder(stubbedDecoder)
        sendParamaterSetsToDecoder(stubbedDecoder)
        sendKeyFrameToDecoder(stubbedDecoder)
        sendKeyFrameToDecoder(stubbedDecoder)
        // Assert that the initial key frame sent before the parameter sets was ignored

        Assert.assertThat(stubbedDecoder.numberOfReceivedPackets(), Is.`is`(IsEqual.equalTo(2)))
    }

    @Test
    @Throws(IOException::class)
    fun testThatTheH264MediaCodecIsConfiguredWithTheSPSSegment() {
        sendParamaterSetsToDecoder(mockedDecoder)

        Assert.assertThat(capturedSPSSegment.value.type, Is.`is`(IsEqual.equalTo(H264NaluType.SPS)))
    }

    @Test
    @Throws(IOException::class)
    fun testThatTheH264MediaCodecIsConfiguredWithThePPSSegment() {
        sendParamaterSetsToDecoder(mockedDecoder)

        Assert.assertThat(capturedPPSSegment.value.type, Is.`is`(IsEqual.equalTo(H264NaluType.PPS)))
    }

    @Test
    @Throws(IOException::class)
    fun testThatTheH264MediaCodecIsStartedAfterBeingConfigured() {
        mockH264Codec.start()
        EasyMock.expectLastCall<Any>()
        EasyMock.replay(mockH264Codec)

        sendParamaterSetsToDecoder(mockedDecoder)

        EasyMock.verify(mockH264Codec)
    }

    @Test
    fun testThatInputBuffersAreNotCached() {
        mockInitialisationOfH264MediaCodec()
        EasyMock.expect(
            mockBuffer.put(
                EasyMock.anyObject<Any>() as ByteArray?, EasyMock.anyInt(), EasyMock.anyInt()
            )
        ).andReturn(mockBuffer).anyTimes()
        EasyMock.replay(mockH264Codec, mockBuffer)

        sendParamaterSetsToDecoder(mockedDecoder)
        mockedDecoder.writePacketToInputBuffer(aKeyFrameH264Packet(), 0)

        EasyMock.verify(mockH264Codec)
    }

    @Test
    @Throws(IOException::class, InterruptedException::class)
    fun testThatTheMediaCodecIsFinalisedWhenTheDecoderIsClosed() {
        mockH264Codec.stop()
        EasyMock.expectLastCall<Any>()
        mockH264Codec.release()
        EasyMock.expectLastCall<Any>()
        stubbedDecoder.close()
    }

    private fun mockInitialisationOfH264MediaCodec() {
        EasyMock.expect(mockH264Codec.getInputBuffer(EasyMock.anyInt())).andReturn(mockBuffer)
        mockH264Codec.start()
        EasyMock.expectLastCall<Any>()
    }

    private fun aKeyFrameH264Packet(): H264Packet =
        H264Packet(H264NaluSegment(H264NaluType.CODED_SLICE_IDR, byteArrayOf()), 1L)

    private fun sendKeyFrameToDecoder(decoder: H264Decoder) {
        decoder.decodeFrameBuffer(testSessionDescription(), ByteBuffer.wrap(aKeyFrameNalUnit()), 200, 200, 1, 1)
    }

    private fun sendParamaterSetsToDecoder(decoder: H264Decoder) {
        decoder.decodeFrameBuffer(testSessionDescription(), ByteBuffer.wrap(aParamaterSetNalUnit()), 200, 200, 1, 1)
    }

    private fun aKeyFrameNalUnit(): ByteArray =
        generateNalUnits(NaluSpec(4, H264NaluType.CODED_SLICE_IDR, 100))

    private fun aParamaterSetNalUnit(): ByteArray = generateNalUnits(
        NaluSpec(4, H264NaluType.PPS, 100),
        NaluSpec(4, H264NaluType.SPS, 100)
    )

    companion object {
        private fun testSessionDescription(): SessionDescription =
            SessionDescription(SessionType.H264, "", "", "", 0, false, 0, 0, 0, 0, 0)
    }
}
