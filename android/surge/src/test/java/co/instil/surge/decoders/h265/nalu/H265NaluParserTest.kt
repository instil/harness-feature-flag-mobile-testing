package co.instil.surge.decoders.h265.nalu

import co.instil.surge.decoders.h265.H265TestUtils
import org.hamcrest.CoreMatchers
import org.hamcrest.MatcherAssert
import org.hamcrest.core.IsEqual
import org.junit.Before
import org.junit.Test
import java.nio.ByteBuffer

class H265NaluParserTest {
    private lateinit var target: H265NaluParser

    @Before
    fun setUp() {
        target = H265NaluParser()
    }

    @Test
    fun shouldReturnNothingWhenParsingAnEmptyBuffer() {
        val buffer = ByteBuffer.wrap(byteArrayOf())

        MatcherAssert.assertThat(target.parseH265NaluSegments(buffer).size, CoreMatchers.`is`(IsEqual.equalTo(0)))
    }

    @Test
    fun shouldReturnNothingWhenParsingABufferContainingNoNalUnits() {
        val buffer = ByteBuffer.wrap(byteArrayOf(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18, 0x29, 0x10, 0x11))

        MatcherAssert.assertThat(target.parseH265NaluSegments(buffer).size, CoreMatchers.`is`(IsEqual.equalTo(0)))
    }

    @Test
    fun shouldParseABufferContainingASingleNalUnit() {
        val buffer = ByteBuffer.wrap(H265TestUtils.generateH265NalUnit(4, H265NaluType.SPS, 10))

        val segments: List<H265NaluSegment> = target.parseH265NaluSegments(buffer)

        MatcherAssert.assertThat(segments.size, CoreMatchers.`is`(IsEqual.equalTo(1)))
    }

    @Test
    fun shouldReturnCorrectNalUnitTypeWhenParsingABufferWithASingleNalUnit() {
        val buffer = ByteBuffer.wrap(H265TestUtils.generateH265NalUnit(4, H265NaluType.SPS, 10))

        val segments: List<H265NaluSegment> = target.parseH265NaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].type, CoreMatchers.`is`(IsEqual.equalTo(H265NaluType.SPS)))
    }

    @Test
    fun shouldReturnCorrectNalUnitPayloadWhenParsingABufferWithASingleNalUnit() {
        val naluBytes = H265TestUtils.generateH265NalUnit(4, H265NaluType.SPS, 10)
        val buffer = ByteBuffer.wrap(naluBytes)

        val segments: List<H265NaluSegment> = target.parseH265NaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].payload, CoreMatchers.`is`(IsEqual.equalTo(naluBytes)))
    }

    @Test
    fun shouldPadNalUnitsWithA3ByteMagicHeaderTo4Bytes() {
        val naluBytes = H265TestUtils.generateH265NalUnit(3, H265NaluType.SPS, 10)
        val buffer = ByteBuffer.wrap(naluBytes)

        val segments: List<H265NaluSegment> = target.parseH265NaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].payloadLength, CoreMatchers.`is`(naluBytes.size + 1))
    }

    @Test
    fun shouldParseMultipleNalUnitsFromABuffer() {
        val nalu1 = H265TestUtils.generateH265NalUnit(4, H265NaluType.PPS, 10)
        val nalu2 = H265TestUtils.generateH265NalUnit(4, H265NaluType.SPS, 10)
        val nalu3 = H265TestUtils.generateH265NalUnit(4, H265NaluType.CODED_SLICE_IDR, 100)
        val multiUnitBuffer = H265TestUtils.joinH265NalUnits(nalu1, nalu2, nalu3)
        val buffer = ByteBuffer.wrap(multiUnitBuffer)

        MatcherAssert.assertThat(target.parseH265NaluSegments(buffer).size, CoreMatchers.`is`(IsEqual.equalTo(3)))
    }

    @Test
    fun shouldReturnCorrectNalUnitTypesWhenParsingMultipleUnitsFromABuffer() {
        val nalu1 = H265TestUtils.generateH265NalUnit(4, H265NaluType.PPS, 10)
        val nalu2 = H265TestUtils.generateH265NalUnit(4, H265NaluType.SPS, 10)
        val nalu3 = H265TestUtils.generateH265NalUnit(4, H265NaluType.CODED_SLICE_IDR, 100)
        val multiUnitBuffer = H265TestUtils.joinH265NalUnits(nalu1, nalu2, nalu3)
        val buffer = ByteBuffer.wrap(multiUnitBuffer)

        val segments: List<H265NaluSegment> = target.parseH265NaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].type, CoreMatchers.`is`(IsEqual.equalTo(H265NaluType.PPS)))
        MatcherAssert.assertThat(segments[1].type, CoreMatchers.`is`(IsEqual.equalTo(H265NaluType.SPS)))
        MatcherAssert.assertThat(segments[2].type, CoreMatchers.`is`(IsEqual.equalTo(H265NaluType.CODED_SLICE_IDR)))
    }

    @Test
    fun shouldReturnCorrectNalUnitPayloadsWhenParsingMultipleUnitsFromABuffer() {
        val nalu1 = H265TestUtils.generateH265NalUnit(4, H265NaluType.PPS, 10)
        val nalu2 = H265TestUtils.generateH265NalUnit(4, H265NaluType.SPS, 10)
        val nalu3 = H265TestUtils.generateH265NalUnit(3, H265NaluType.CODED_SLICE_IDR, 100)
        val multiUnitBuffer = H265TestUtils.joinH265NalUnits(nalu1, nalu2, nalu3)
        val buffer = ByteBuffer.wrap(multiUnitBuffer)

        val segments: List<H265NaluSegment> = target.parseH265NaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].payload, CoreMatchers.`is`(IsEqual.equalTo(nalu1)))
        MatcherAssert.assertThat(segments[1].payload, CoreMatchers.`is`(IsEqual.equalTo(nalu2)))
        MatcherAssert.assertThat(segments[2].payload, CoreMatchers.`is`(IsEqual.equalTo(nalu3)))
    }
}