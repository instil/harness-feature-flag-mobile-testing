#ifndef H264NALUSEGMENT_H
#define H264NALUSEGMENT_H

#include <QByteArray>
#include "H264NaluType.h"

struct H264NaluSegment {
    const H264NaluType type;
    const QByteArray payload;

    H264NaluSegment(int rawType, QByteArray payload) : type(GetTypeFromCode(rawType)), payload(payload) {}

    int PayloadLength() {
        return payload.size();
    }

private:
    H264NaluType GetTypeFromCode(int rawType) {
        H264NaluType type = H264NaluType::UNSPECIFIED;
        switch (rawType) {
            case 1:
                type = H264NaluType::CODED_SILCE_NON_IDR;
                break;
            case 5:
                type = H264NaluType::CODED_SLICE_IDR;
                break;
            case 7:
                type = H264NaluType::SPS;
                break;
            case 8:
                type = H264NaluType::PPS;
                break;
            default:
                break;
        }
        return type;
    }
};

#endif // H264NALUSEGMENT_H
