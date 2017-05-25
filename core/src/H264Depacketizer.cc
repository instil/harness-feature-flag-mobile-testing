// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "H264Depacketizer.h"
#include "Base64.h"

void Surge::H264Depacketizer::ProcessPacket(const RtpPacket *packet, const bool isFirstPayload) {

    if (packet == nullptr || !packet->HasPayload()) {
        ERROR("No payload, skipping packet processing");
        return;
    }

    const unsigned char *payloadData = packet->PayloadData();
    size_t payloadLength = packet->PayloadLength();

    if (isFirstPayload) {
        std::string config = sessionDescription.GetFmtpH264ConfigParameters();

        size_t pos = config.find(",");
        std::string encodedFirstNalu = config.substr(0, pos);
        std::string encodedSecondNalu = config.substr(pos + 1, config.length());

        std::string firstNalu = SurgeUtil::Base64Decode(encodedFirstNalu);
        std::string secondNalu = SurgeUtil::Base64Decode(encodedSecondNalu);

        AppendFourByteNaluHeaderToFrame();
        AppendBytesToFrameBuffer((const unsigned char *)firstNalu.c_str(), firstNalu.length());

        AppendFourByteNaluHeaderToFrame();
        AppendBytesToFrameBuffer((const unsigned char *)secondNalu.c_str(), secondNalu.length());

        SetWidth(720);
        SetHeight(480);
    }

    int payload_type = GetH264NaluTypeFromByte(payloadData[0]);

    switch(payload_type) {
    case 0:
        AppendBytesToFrameBuffer(payloadData, payloadLength);
        break;
    case 7:
    case 8:
        AppendThreeByteNaluHeaderToFrame();
        AppendBytesToFrameBuffer(payloadData, payloadLength);
        break;
    case 24:
        AppendThreeByteNaluHeaderToFrame();
        AppendBytesToFrameBuffer(payloadData + 1, payloadLength - 1);
        break;
    case 25:
    case 26:
    case 27:
        AppendThreeByteNaluHeaderToFrame();
        AppendBytesToFrameBuffer(payloadData + 3, payloadLength - 3);
        break;
    case 28:
    case 29: {
        unsigned char startBit = payloadData[1] >> 7;
        if (startBit) {
            unsigned char *payloadDataWithHeader = (unsigned char*)malloc(payloadLength - 1);
            memcpy(payloadDataWithHeader, payloadData + 1, payloadLength - 1);

            unsigned char header = (payloadData[0] & 0xE0) + (payloadData[1] & 0x1F);
            payloadDataWithHeader[0] = header;

            AppendThreeByteNaluHeaderToFrame();
            AppendBytesToFrameBuffer(payloadDataWithHeader, payloadLength - 1);

            free(payloadDataWithHeader);
        } else {
            AppendBytesToFrameBuffer(payloadData + 2, payloadLength - 2);
        }
    }
        break;
        
    default:
        AppendThreeByteNaluHeaderToFrame();
        AppendBytesToFrameBuffer(payloadData, payloadLength);
        break;
    }
    
}
