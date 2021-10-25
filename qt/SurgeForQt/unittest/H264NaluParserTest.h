#ifndef H264NALUPARSERTEST_H
#define H264NALUPARSERTEST_H

#include <QtTest>
#include <QBuffer>
#include "H264NaluParser.h"
#include "H264NaluType.h"
#include "TestSuite.h"

class H264NaluParserTest : public TestSuite {
    Q_OBJECT

private:
    QByteArray GenerateH264NalUnitBytes(int headerLength, H264NaluType type, int payloadLength);

private slots:
    void ShouldReturnEmptyVectorWhenParsingEmptyBuffer();

    void ShouldReturnEmptyVectorWhenParsingABufferContainingNoNalUnits();

    void ShouldParseNalUnitsWith3ByteHeader();

    void ShouldParseNalUnitsWith4ByteHeader();
};

static H264NaluParserTest TEST_H264NALUPARSER;

#endif // H264NALUPARSERTEST_H
