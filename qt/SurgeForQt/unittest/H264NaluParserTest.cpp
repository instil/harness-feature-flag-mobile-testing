#include "H264NaluParserTest.h"

void H264NaluParserTest::ShouldReturnEmptyVectorWhenParsingEmptyBuffer() {
    auto parser = H264NaluParser();
    auto byteArray = QByteArray();
    QBuffer buffer(&byteArray);

    auto actual = parser.ParseNaluSegments(buffer);

    QCOMPARE(actual.size(), 0);
}

void H264NaluParserTest::ShouldReturnEmptyVectorWhenParsingABufferContainingNoNalUnits() {
    auto parser = H264NaluParser();
    auto byteArray = QByteArray();
    QBuffer buffer(&byteArray);

    auto actual = parser.ParseNaluSegments(buffer);

    QCOMPARE(actual.size(), 0);
}

void H264NaluParserTest::ShouldParseNalUnitsWith3ByteHeader() {
    auto parser = H264NaluParser();
    auto byteArray = GenerateH264NalUnitBytes(3, H264NaluType::PPS, 10);
    QBuffer buffer(&byteArray);
    auto segments = parser.ParseNaluSegments(buffer);

    QCOMPARE(segments.size(), 1);
    QCOMPARE(segments.at(0).type, H264NaluType::PPS);
}

void H264NaluParserTest::ShouldParseNalUnitsWith4ByteHeader() {
    auto parser = H264NaluParser();
    auto byteArray = GenerateH264NalUnitBytes(4, H264NaluType::SPS, 10);
    QBuffer buffer(&byteArray);
    auto segments = parser.ParseNaluSegments(buffer);

    QCOMPARE(segments.size(), 1);
    QCOMPARE(segments.at(0).type, H264NaluType::SPS);
}

QByteArray H264NaluParserTest::GenerateH264NalUnitBytes(int headerLength, H264NaluType type, int payloadLength) {
    char zerobyte = 0x00;
    auto payload = QByteArray();
    for (int i = 0; i < headerLength - 1; i++) {
        payload.append(zerobyte);
    }
    payload.append(0x01);
    payload.append((uint8_t)type);
    for (int i = 0; i < payloadLength; i++) {
        payload.append((char)i);
    }
    return payload;
}
