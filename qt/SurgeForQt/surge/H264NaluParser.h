#ifndef H264NALUPARSER_H
#define H264NALUPARSER_H

#include <QBuffer>
#include <vector>
#include "NaluMagicByteHeader.h"
#include "H264NaluSegment.h"

class H264NaluParser {
public:
    H264NaluParser();

    std::vector <H264NaluSegment> ParseNaluSegments(QBuffer &buffer);

private:
    std::vector <NaluMagicByteHeader> FindMagicByteHeadersInStream(QByteArray &stream);

    int LengthOfMagicByteSequenceAtPosition(QByteArray &byteArray, int position);

    std::vector <H264NaluSegment> ExtractNalUnitsInStream(std::vector <NaluMagicByteHeader> headers, QByteArray &stream);

private:
    static const int MINIMUM_MAGIC_HEADER_LENGTH = 3;
    static const int MAXIMUM_MAGIC_HEADER_LENGTH = 4;
    static const int MINIMUM_NAL_UNIT_LENGTH = 6;
};

#endif // H264NALUPARSER_H
