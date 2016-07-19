package co.instil.surge.decoders.h264.nalu;

import org.junit.Before;
import org.junit.Test;

import java.nio.ByteBuffer;
import java.util.List;

import static co.instil.surge.decoders.h264.H264TestUtils.generateNalUnitBytes;
import static co.instil.surge.decoders.h264.H264TestUtils.joinNalUnits;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.core.IsEqual.equalTo;

/**
 * Created by mattmccomb on 13/07/16.
 */
public class NaluParserTest {

    private NaluParser parser;

    @Before
    public void setUp() {
        parser = new NaluParser();
    }

    @Test
    public void testThatNothingIsReturnedWhenParsingAnEmptyBuffer() throws Exception {
        ByteBuffer buffer = ByteBuffer.wrap(new byte[]{});
        assertThat(parser.parseNaluSegments(buffer).size(), is(equalTo(0)));
    }

    @Test
    public void testThatNothingIsReturningWhenParsingABufferContainingNoNalUnits() throws Exception {
        // Parse a random string of bytes (containing no NAL units e.g. there is no 0x000001 or
        // 0x00000001 magic byte header to indicate the start of a NAL unit)
        ByteBuffer buffer = ByteBuffer.wrap(new byte[] { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18, 0x29, 0x10, 0x11});
        assertThat(parser.parseNaluSegments(buffer).size(), is(equalTo(0)));
    }

    @Test
    public void testThatABufferContaingASingleNalUnitCanBeParsed() throws Exception {
        ByteBuffer buffer = ByteBuffer.wrap(generateNalUnitBytes(4, NaluType.SPS, 10));
        List<NaluSegment> segments = parser.parseNaluSegments(buffer);
        assertThat(segments.size(), is(equalTo(1)));
    }

    @Test
    public void testThatNalUnitTypeIsCorrectWhenParsingABufferWithASingleNalUnit() throws Exception {
        ByteBuffer buffer = ByteBuffer.wrap(generateNalUnitBytes(4, NaluType.SPS, 10));
        List<NaluSegment> segments = parser.parseNaluSegments(buffer);
        assertThat(segments.get(0).getType(), is(equalTo(NaluType.SPS)));
    }

    @Test
    public void testThatNalUnitPayloadIsCorrectWhenParsingABufferWithASingleNalUnit() throws Exception {
        byte[] naluBytes = generateNalUnitBytes(4, NaluType.SPS, 10);
        ByteBuffer buffer = ByteBuffer.wrap(naluBytes);
        List<NaluSegment> segments = parser.parseNaluSegments(buffer);
        assertThat(segments.get(0).getPayload(), is(equalTo(naluBytes)));
    }

    @Test
    public void testThatNalUnitsWithA3ByteMagicHeaderArePaddedTo4Bytes() throws Exception {
        byte[] naluBytes = generateNalUnitBytes(3, NaluType.SPS, 10);
        ByteBuffer buffer = ByteBuffer.wrap(naluBytes);
        List<NaluSegment> segments = parser.parseNaluSegments(buffer);
        assertThat(segments.get(0).getPayloadLength(), is(naluBytes.length + 1));
    }

    @Test
    public void testThatMultipleNalUnitsCanBeParsedFromABuffer() throws Exception {
        byte[] nalu1 = generateNalUnitBytes(4, NaluType.PPS, 10);
        byte[] nalu2 = generateNalUnitBytes(4, NaluType.SPS, 10);
        byte[] nalu3 = generateNalUnitBytes(4, NaluType.CODED_SLICE_IDR, 100);
        byte[] multiUnitBuffer = joinNalUnits(nalu1, nalu2, nalu3);
        ByteBuffer buffer = ByteBuffer.wrap(multiUnitBuffer);
        assertThat(parser.parseNaluSegments(buffer).size(), is(equalTo(3)));
    }

    @Test
    public void testThatNalUnitTypesAreCorrectWhenParsingMultipleUnitsFromABuffer() throws Exception {
        byte[] nalu1 = generateNalUnitBytes(4, NaluType.PPS, 10);
        byte[] nalu2 = generateNalUnitBytes(4, NaluType.SPS, 10);
        byte[] nalu3 = generateNalUnitBytes(4, NaluType.CODED_SLICE_IDR, 100);
        byte[] multiUnitBuffer = joinNalUnits(nalu1, nalu2, nalu3);
        ByteBuffer buffer = ByteBuffer.wrap(multiUnitBuffer);
        List<NaluSegment> segments = parser.parseNaluSegments(buffer);
        assertThat(segments.get(0).getType(), is(equalTo(NaluType.PPS)));
        assertThat(segments.get(1).getType(), is(equalTo(NaluType.SPS)));
        assertThat(segments.get(2).getType(), is(equalTo(NaluType.CODED_SLICE_IDR)));
    }

    @Test
    public void testThatNalUnitPayloadsAreCorrectWhenParsingMultipleUnitsFromABuffer() throws Exception {
        byte[] nalu1 = generateNalUnitBytes(4, NaluType.PPS, 10);
        byte[] nalu2 = generateNalUnitBytes(4, NaluType.SPS, 10);
        byte[] nalu3 = generateNalUnitBytes(4, NaluType.CODED_SLICE_IDR, 100);
        byte[] multiUnitBuffer = joinNalUnits(nalu1, nalu2, nalu3);
        ByteBuffer buffer = ByteBuffer.wrap(multiUnitBuffer);
        List<NaluSegment> segments = parser.parseNaluSegments(buffer);
        assertThat(segments.get(0).getPayload(), is(equalTo(nalu1)));
        assertThat(segments.get(1).getPayload(), is(equalTo(nalu2)));
        assertThat(segments.get(2).getPayload(), is(equalTo(nalu3)));
    }


}
