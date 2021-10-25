#define EMPTY_NAL_UNIT_LIST std::vector<H264NaluSegment>()

#include "H264NaluParser.h"

H264NaluParser::H264NaluParser() {

}

std::vector<H264NaluSegment> H264NaluParser::ParseNaluSegments(QBuffer &buffer) {
    auto stream = buffer.data();

    if (stream.length() <= MINIMUM_NAL_UNIT_LENGTH) {
        return EMPTY_NAL_UNIT_LIST;
    }

    auto headers = FindMagicByteHeadersInStream(stream);
    return ExtractNalUnitsInStream(headers, stream);
}

std::vector<NaluMagicByteHeader> H264NaluParser::FindMagicByteHeadersInStream(QByteArray &stream) {
    std::vector<NaluMagicByteHeader> headers;
    for (int i = 0; i < stream.size() - MINIMUM_MAGIC_HEADER_LENGTH; ++i) {
        int magicByteLength = LengthOfMagicByteSequenceAtPosition(stream, i);
        bool isStartOfNalUnit = magicByteLength != -1;
        if (isStartOfNalUnit) {
            headers.push_back(NaluMagicByteHeader(i, magicByteLength));
            i += magicByteLength;
        }
    }
    return headers;
}

int H264NaluParser::LengthOfMagicByteSequenceAtPosition(QByteArray &byteArray, int position) {
    auto startsWithZeroBytes = byteArray.at(position) == 0x00 && byteArray.at(position + 1) == 0x00;
    if (startsWithZeroBytes && byteArray.at(position + 2) == 0x01) {
        return MINIMUM_MAGIC_HEADER_LENGTH;
    } else if (startsWithZeroBytes && byteArray.at(position + 2) == 0x00 && byteArray.at(position + 3) == 0x01) {
        return MAXIMUM_MAGIC_HEADER_LENGTH;
    }

    return -1;
}

std::vector<H264NaluSegment> H264NaluParser::ExtractNalUnitsInStream(std::vector<NaluMagicByteHeader> headers, QByteArray &stream) {
    std::vector<H264NaluSegment> segments;
    for (size_t i = 0; i < headers.size(); i++) {
        auto header = headers.at(i);
        auto isLastHeader = i == headers.size() - 1;
        auto dataStart = header.position + header.length;
        auto nalUnit = stream.at(dataStart) & 0x1F;
        QByteArray payload;
        if (!isLastHeader) {
            auto nextHeader = headers.at(i + 1);
            auto length = nextHeader.position - dataStart;
            payload = stream.mid(dataStart, length);
        } else {
            auto length = stream.size() - dataStart;
            payload = stream.mid(dataStart, length);
        }
        segments.push_back(H264NaluSegment(nalUnit, payload));
    }
    return segments;
}
