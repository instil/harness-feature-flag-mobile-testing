// Copyright (c) 2020 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "H265Depacketizer.h"

#include "Base64.h"

void Surge::H265Depacketizer::ProcessPacket(const RtpPacket *packet, const bool isFirstPayload) {
    if (packet == nullptr || !packet->HasPayload()) {
        ERROR("No payload, skipping packet processing");
        return;
    }

    const unsigned char *payloadData = packet->PayloadData();
    size_t payloadLength = packet->PayloadLength();

    if (isFirstPayload) {
        auto base64VpsNalUnit = sessionDescription.GetFmtpH265Vps();
        auto base64SpsNalUnit = sessionDescription.GetFmtpH265Sps();
        auto base64PpsNalUnit = sessionDescription.GetFmtpH265Pps();

        AppendBase64EncodedNalUnit(base64VpsNalUnit);
        AppendBase64EncodedNalUnit(base64SpsNalUnit);
        AppendBase64EncodedNalUnit(base64PpsNalUnit);
    }

    int payloadType = GetH265NaluTypeFromByte(payloadData[0]);
    switch (payloadType) {
        case 49:
            HandleFragmentationPacket(packet);
            break;
        default:
            AppendNaluHeaderToFrame(payloadType);
            AppendBytesToFrameBuffer(payloadData, payloadLength);
    }
}

void Surge::H265Depacketizer::HandleFragmentationPacket(const RtpPacket *packet) {
    const unsigned char *payloadData = packet->PayloadData();
    int payloadType = GetH265NaluTypeFromByte(payloadData[0]);
    bool isFirstPartOfFragmentedPacket = payloadData[2] & 0x80;

    if (isFirstPartOfFragmentedPacket) {
        AppendNaluHeaderToFrame(payloadType);
        auto readNalUnitHeader = ConstructNalUnitHeaderFromFragmentationPacket(packet);
        AppendBytesToFrameBuffer(readNalUnitHeader, 2);
        free(readNalUnitHeader);
    }

    auto fragmentedNalUnitBody = packet->PayloadData() + 3;
    auto fragmentedNalUnitBodyLength = packet->PayloadLength() - 3;

    AppendBytesToFrameBuffer(fragmentedNalUnitBody, fragmentedNalUnitBodyLength);
}

void Surge::H265Depacketizer::AppendBase64EncodedNalUnit(std::string base64NalUnit) {
    if (!base64NalUnit.empty()) {
        auto nalUnit = SurgeUtil::Base64Decode(base64NalUnit);

        AppendFourByteNaluHeaderToFrame();
        AppendBytesToFrameBuffer((const unsigned char *)nalUnit.c_str(), nalUnit.length());
    }
}

unsigned char * Surge::H265Depacketizer::ConstructNalUnitHeaderFromFragmentationPacket(const RtpPacket *packet) {
    auto fragmentedPacket = (unsigned char *)packet->PayloadData();

    unsigned char *nalUnitHeader = (unsigned char *)malloc(2);
    memcpy(nalUnitHeader, fragmentedPacket, 2);

    uint8_t nalUnitType = fragmentedPacket[2] & 0x3F;

    nalUnitHeader[0] &= 0x9f;
    nalUnitHeader[0] |= (nalUnitType << 1);

    return nalUnitHeader;
}
