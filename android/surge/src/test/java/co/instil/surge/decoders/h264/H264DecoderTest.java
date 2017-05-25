/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.h264;

import android.annotation.TargetApi;
import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.os.Build;
import android.view.Surface;

import org.easymock.Capture;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.nio.ByteBuffer;

import co.instil.surge.decoders.MediaCodecFactory;
import co.instil.surge.decoders.h264.nalu.NaluParser;
import co.instil.surge.decoders.h264.nalu.NaluSegment;
import co.instil.surge.decoders.h264.nalu.NaluType;
import co.instil.surge.device.DeviceExaminer;

import static co.instil.surge.decoders.h264.H264TestUtils.generateNalUnits;
import static org.easymock.EasyMock.anyInt;
import static org.easymock.EasyMock.anyObject;
import static org.easymock.EasyMock.capture;
import static org.easymock.EasyMock.createMock;
import static org.easymock.EasyMock.eq;
import static org.easymock.EasyMock.expect;
import static org.easymock.EasyMock.expectLastCall;
import static org.easymock.EasyMock.replay;
import static org.easymock.EasyMock.verify;
import static org.hamcrest.core.Is.is;
import static org.hamcrest.core.IsEqual.equalTo;
import static org.junit.Assert.assertThat;


/**
 * Created by mattmccomb on 08/07/16.
 */
public class H264DecoderTest {

    private Surface surface;
    private StubH264Decoder stubbedDecoder;
    private MediaCodec mockH264Codec;
    private ByteBuffer mockBuffer;
    private MediaCodecFactory mockMediaCodecFactory;
    private DeviceExaminer mockDeviceExaminer;
    private StubH264Decoder mockedDecoder;
    private Capture<NaluSegment> capturedPPSSegment, capturedSPSSegment;

    @Before
    public void setUp() throws Exception {
        surface = new Surface(new SurfaceTexture(0));
        stubbedDecoder = new StubH264Decoder(surface);
        capturedSPSSegment = Capture.newInstance();
        capturedPPSSegment = Capture.newInstance();
        mockBuffer = createMock(ByteBuffer.class);
        mockDeviceExaminer = createMock(DeviceExaminer.class);
        mockH264Codec = createMock(MediaCodec.class);
        mockMediaCodecFactory = createMock(MediaCodecFactory.class);
        mockedDecoder = new StubH264Decoder(surface, mockMediaCodecFactory, new NaluParser(), mockDeviceExaminer);
        expect(mockMediaCodecFactory.createH264DecoderWithParameters(
                capture(capturedSPSSegment),
                capture(capturedPPSSegment),
                eq(surface),
                anyInt(),
                anyInt()))
                .andReturn(mockH264Codec);
        replay(mockMediaCodecFactory);
    }

    @Test
    public void testThatTheDecoderIgnoresFramesUntilItReceivesParameterSets() {
        sendKeyFrameToDecoder(stubbedDecoder);
        sendParamaterSetsToDecoder(stubbedDecoder);
        sendKeyFrameToDecoder(stubbedDecoder);
        sendKeyFrameToDecoder(stubbedDecoder);
        // Assert that the initial key frame sent before the parameter sets was ignored
        assertThat(stubbedDecoder.numberOfReceivedPackets(), is(equalTo(2)));
    }

    @Test
    public void testThatTheH264MediaCodecIsConfiguredWithTheSPSSegment() throws IOException {
        sendParamaterSetsToDecoder(mockedDecoder);
        assertThat(capturedSPSSegment.getValue().getType(), is(equalTo(NaluType.SPS)));
    }

    @Test
    public void testThatTheH264MediaCodecIsConfiguredWithThePPSSegment() throws IOException {
        sendParamaterSetsToDecoder(mockedDecoder);
        assertThat(capturedPPSSegment.getValue().getType(), is(equalTo(NaluType.PPS)));
    }

    @Test
    public void testThatTheH264MediaCodecIsStartedAfterBeingConfigured() throws IOException {
        mockH264Codec.start();
        expectLastCall();
        replay(mockH264Codec);
        sendParamaterSetsToDecoder(mockedDecoder);
        verify(mockH264Codec);
    }

    @Test
    public void testThatInputBuffersAreCachedFromTheMediaCodecOnPreLollipopDevices() throws IOException {
        mockInitialisationOfH264MediaCodecOnPreLollipopDevice();
        expect(mockBuffer.put((byte[])anyObject(), anyInt(), anyInt())).andReturn(mockBuffer).anyTimes();
        replay(mockH264Codec, mockBuffer);
        sendParamaterSetsToDecoder(mockedDecoder);
        mockedDecoder.writePacketToInputBuffer(aKeyFrameH264Packet(), 0);
        verify(mockBuffer);
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Test
    public void testThatInputBuffersAreNotCachedOnLollipopDevices() {
        mockInitialisationOfH264MediaCodecOnPostLollipopDevice();
        expect(mockBuffer.put((byte[])anyObject(), anyInt(), anyInt())).andReturn(mockBuffer).anyTimes();
        replay(mockH264Codec, mockBuffer);
        sendParamaterSetsToDecoder(mockedDecoder);
        mockedDecoder.writePacketToInputBuffer(aKeyFrameH264Packet(), 0);
        verify(mockH264Codec);
    }

    @Test
    public void testThatDecoderInputBuffersAreClearedBeforeBeingWrittenTo() throws IOException {
        mockInitialisationOfH264MediaCodecOnPreLollipopDevice();
        expect(mockBuffer.put((byte[])anyObject(), anyInt(), anyInt())).andReturn(mockBuffer).anyTimes();
        replay(mockH264Codec, mockBuffer);
        sendParamaterSetsToDecoder(mockedDecoder);
        mockedDecoder.writePacketToInputBuffer(aKeyFrameH264Packet(), 0);
        verify(mockBuffer.clear());
    }

    @Test
    public void testThatTheMediaCodecIsFinalisedWhenTheDecoderIsClosed() throws IOException, InterruptedException {
        mockH264Codec.stop();
        expectLastCall();
        mockH264Codec.release();
        expectLastCall();
        stubbedDecoder.close();
    }


    private void mockInitialisationOfH264MediaCodecOnPreLollipopDevice() {
        mockDeviceVersionToBePreLollipop();
        expect(mockH264Codec.getInputBuffers()).andReturn(new ByteBuffer[] {mockBuffer}).times(2);
        mockH264Codec.start();
        expectLastCall();
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void mockInitialisationOfH264MediaCodecOnPostLollipopDevice() {
        mockDeviceVersionToBeLollipopOrNewer();
        expect(mockH264Codec.getInputBuffer(anyInt())).andReturn(mockBuffer);
        mockH264Codec.start();
        expectLastCall();
    }

    private void mockDeviceVersionToBePreLollipop() {
        expect(mockDeviceExaminer.isPreLollipopDevice()).andReturn(true).anyTimes();
        replay(mockDeviceExaminer);
    }

    private void mockDeviceVersionToBeLollipopOrNewer() {
        expect(mockDeviceExaminer.isPreLollipopDevice()).andReturn(false).anyTimes();
        replay(mockDeviceExaminer);
    }

    private H264Packet aKeyFrameH264Packet() {
        return new H264Packet(new NaluSegment(NaluType.CODED_SLICE_IDR, new byte[] {}), 1L);
    }

    private void sendKeyFrameToDecoder(H264Decoder decoder) {
        decoder.decodeFrameBuffer(null, ByteBuffer.wrap(aKeyFrameNalUnit()), 200, 200, 1, 1);
    }

    private void sendParamaterSetsToDecoder(H264Decoder decoder) {
        decoder.decodeFrameBuffer(null, ByteBuffer.wrap(aParamaterSetNalUnit()), 200, 200, 1, 1);
    }

    private byte[] aKeyFrameNalUnit() {
        return generateNalUnits(new H264TestUtils.NaluSpec(4, NaluType.CODED_SLICE_IDR, 100));
    }

    private byte[] aParamaterSetNalUnit() {
        return generateNalUnits(
                new H264TestUtils.NaluSpec(4, NaluType.PPS, 100),
                new H264TestUtils.NaluSpec(4, NaluType.SPS, 100));
    }

}
