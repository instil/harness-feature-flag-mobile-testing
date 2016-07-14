package co.instil.surge.decoders.h264;

import android.graphics.SurfaceTexture;
import android.view.Surface;
import co.instil.surge.decoders.MediaCodecFactory;
import co.instil.surge.decoders.h264.nalu.NaluParser;
import co.instil.surge.decoders.h264.nalu.NaluType;
import org.junit.Before;
import org.junit.Test;

import java.nio.ByteBuffer;

import static co.instil.surge.decoders.h264.H26TestUtils.generateNalUnits;
import static org.easymock.EasyMock.mock;
import static org.hamcrest.core.Is.is;
import static org.hamcrest.core.IsEqual.equalTo;
import static org.hamcrest.core.IsNull.nullValue;
import static org.junit.Assert.assertThat;


/**
 * Created by mattmccomb on 08/07/16.
 */
public class H264DecoderTest {

    private Surface surface;

    private NaluParser naluParser;
    private MediaCodecFactory mediaCodecFactory;
    private StubH264Decoder h264Decoder;

    @Before
    public void setUp() {
        surface = new Surface(new SurfaceTexture(0));
        naluParser = mock(NaluParser.class);
        mediaCodecFactory = mock(MediaCodecFactory.class);
        h264Decoder = new StubH264Decoder(surface);
    }

    @Test
    public void testThatMediaCodecIsInitiallyNull() {
        assertThat(h264Decoder.getMediaCodec(), is(nullValue()));
    }

    @Test
    public void testThatTheDecoderIgnoresFramesUntilItReceivesParameterSets() {
        sendKeyFrameToDecoder();
        sendParamaterSetsToDecoder();
        sendKeyFrameToDecoder();
        sendKeyFrameToDecoder();
        // Assert that the initial key frame sent before the parameter sets was ignored
        assertThat(h264Decoder.numberOfReceivedPackets(), is(equalTo(2)));
    }



    private void sendKeyFrameToDecoder() {
        h264Decoder.decodeFrameBuffer(null, ByteBuffer.wrap(aKeyFrameNalUnit()), 200, 200, 1, 1);
    }

    private void sendParamaterSetsToDecoder() {
        h264Decoder.decodeFrameBuffer(null, ByteBuffer.wrap(aParamaterSetNalUnit()), 200, 200, 1, 1);
    }

    private byte[] aKeyFrameNalUnit() {
        return generateNalUnits(new H26TestUtils.NaluSpec(4, NaluType.PPS, 100));
    }

    private byte[] aParamaterSetNalUnit() {
        return generateNalUnits(
                new H26TestUtils.NaluSpec(4, NaluType.PPS, 100),
                new H26TestUtils.NaluSpec(4, NaluType.SPS, 100));
    }

}
