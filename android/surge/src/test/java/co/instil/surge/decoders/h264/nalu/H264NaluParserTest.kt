/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.decoders.h264.nalu

import co.instil.surge.decoders.h264.H264TestUtils
import org.hamcrest.CoreMatchers
import org.hamcrest.MatcherAssert
import org.hamcrest.core.IsEqual
import org.junit.Before
import org.junit.Test
import java.lang.Exception
import java.nio.ByteBuffer
import kotlin.Throws

/**
 * Created by mattmccomb on 13/07/16.
 */
class H264NaluParserTest {
    private lateinit var target: H264NaluParser

    @Before
    fun setUp() {
        target = H264NaluParser()
    }

    @Test
    @Throws(Exception::class)
    fun testThatNothingIsReturnedWhenParsingAnEmptyBuffer() {
        val buffer = ByteBuffer.wrap(byteArrayOf())

        MatcherAssert.assertThat(target.parseNaluSegments(buffer).size, CoreMatchers.`is`(IsEqual.equalTo(0)))
    }

    @Test
    @Throws(Exception::class)
    fun testThatNothingIsReturningWhenParsingABufferContainingNoNalUnits() {
        // Parse a random string of bytes (containing no NAL units e.g. there is no 0x000001 or
        // 0x00000001 magic byte header to indicate the start of a NAL unit)
        val buffer = ByteBuffer.wrap(byteArrayOf(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18, 0x29, 0x10, 0x11))

        MatcherAssert.assertThat(target.parseNaluSegments(buffer).size, CoreMatchers.`is`(IsEqual.equalTo(0)))
    }

    @Test
    @Throws(Exception::class)
    fun testThatABufferContainingASingleNalUnitCanBeParsed() {
        val buffer = ByteBuffer.wrap(H264TestUtils.generateNalUnitBytes(4, H264NaluType.SPS, 10))

        val segments = target.parseNaluSegments(buffer)

        MatcherAssert.assertThat(segments.size, CoreMatchers.`is`(IsEqual.equalTo(1)))
    }

    @Test
    @Throws(Exception::class)
    fun testThatNalUnitTypeIsCorrectWhenParsingABufferWithASingleNalUnit() {
        val buffer = ByteBuffer.wrap(H264TestUtils.generateNalUnitBytes(4, H264NaluType.SPS, 10))

        val segments = target.parseNaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].type, CoreMatchers.`is`(IsEqual.equalTo(H264NaluType.SPS)))
    }

    @Test
    @Throws(Exception::class)
    fun testThatNalUnitPayloadIsCorrectWhenParsingABufferWithASingleNalUnit() {
        val naluBytes = H264TestUtils.generateNalUnitBytes(4, H264NaluType.SPS, 10)
        val buffer = ByteBuffer.wrap(naluBytes)

        val segments = target.parseNaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].payload, CoreMatchers.`is`(IsEqual.equalTo(naluBytes)))
    }

    @Test
    @Throws(Exception::class)
    fun testThatNalUnitsWithA3ByteMagicHeaderArePaddedTo4Bytes() {
        val naluBytes = H264TestUtils.generateNalUnitBytes(3, H264NaluType.SPS, 10)
        val buffer = ByteBuffer.wrap(naluBytes)

        val segments = target.parseNaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].payloadLength, CoreMatchers.`is`(naluBytes.size + 1))
    }

    @Test
    @Throws(Exception::class)
    fun testThatMultipleNalUnitsCanBeParsedFromABuffer() {
        val nalu1 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.PPS, 10)
        val nalu2 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.SPS, 10)
        val nalu3 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.CODED_SLICE_IDR, 100)
        val multiUnitBuffer = H264TestUtils.joinNalUnits(nalu1, nalu2, nalu3)
        val buffer = ByteBuffer.wrap(multiUnitBuffer)

        MatcherAssert.assertThat(target.parseNaluSegments(buffer).size, CoreMatchers.`is`(IsEqual.equalTo(3)))
    }

    @Test
    @Throws(Exception::class)
    fun testThatNalUnitTypesAreCorrectWhenParsingMultipleUnitsFromABuffer() {
        val nalu1 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.PPS, 10)
        val nalu2 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.SPS, 10)
        val nalu3 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.CODED_SLICE_IDR, 100)
        val multiUnitBuffer = H264TestUtils.joinNalUnits(nalu1, nalu2, nalu3)
        val buffer = ByteBuffer.wrap(multiUnitBuffer)

        val segments = target.parseNaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].type, CoreMatchers.`is`(IsEqual.equalTo(H264NaluType.PPS)))
        MatcherAssert.assertThat(segments[1].type, CoreMatchers.`is`(IsEqual.equalTo(H264NaluType.SPS)))
        MatcherAssert.assertThat(segments[2].type, CoreMatchers.`is`(IsEqual.equalTo(H264NaluType.CODED_SLICE_IDR)))
    }

    @Test
    @Throws(Exception::class)
    fun testThatNalUnitPayloadsAreCorrectWhenParsingMultipleUnitsFromABuffer() {
        val nalu1 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.PPS, 10)
        val nalu2 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.SPS, 10)
        val nalu3 = H264TestUtils.generateNalUnitBytes(4, H264NaluType.CODED_SLICE_IDR, 100)
        val multiUnitBuffer = H264TestUtils.joinNalUnits(nalu1, nalu2, nalu3)
        val buffer = ByteBuffer.wrap(multiUnitBuffer)

        val segments = target.parseNaluSegments(buffer)

        MatcherAssert.assertThat(segments[0].payload, CoreMatchers.`is`(IsEqual.equalTo(nalu1)))
        MatcherAssert.assertThat(segments[1].payload, CoreMatchers.`is`(IsEqual.equalTo(nalu2)))
        MatcherAssert.assertThat(segments[2].payload, CoreMatchers.`is`(IsEqual.equalTo(nalu3)))
    }
}
