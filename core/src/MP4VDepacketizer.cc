// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "MP4VDepacketizer.h"
#include "BitstreamReader.h"

#include <math.h>

void Surge::MP4VDepacketizer::ProcessPacket(const RtpPacket *packet,
                                            const bool isFirstPayload) {

    if (isFirstPayload) {
        std::string config = sessionDescription.GetFmtpConfigParameters();
        std::vector<unsigned char> configBytes;

        size_t i;
        for (i = 0; i < config.length(); i += 2) {
            std::string currentByte = config.substr(i, 2);
            unsigned char wrappedByte = (unsigned char) strtol(currentByte.c_str(), NULL, 16);
            configBytes.push_back(wrappedByte);
        }

        AppendBytesToFrameBuffer(&(configBytes[0]), configBytes.size());
        ExtractDimensionsFromVosHeader(configBytes);
    }

    const unsigned char *payloadData = packet->PayloadData();
    size_t payloadLength = packet->PayloadLength();

    AppendBytesToFrameBuffer(payloadData, payloadLength);
}


/**
 * See the VisualObjectSequence definition in the ISO/IEC 14496-2 (MPEG4 Visual) specification.
 */
void Surge::MP4VDepacketizer::ExtractDimensionsFromVosHeader(std::vector<unsigned char> config) {
    uint8_t *vosHeader = config.data();
    size_t size = config.size();
    for (int i = 0; i < frameBuffer->size() - 4; i++) {
        if (IsVideoObjectLayerStartCode(vosHeader + i)) {
            ExtractDimensionsFromVolHeader(vosHeader + i + 4, size - i - 4);
            return;
        }
    }
    SetWidth(0);
    SetHeight(0);
}

bool Surge::MP4VDepacketizer::IsVideoObjectLayerStartCode(uint8_t *descriptor) {
    const uint8_t startCodePrefix[] = { 0x00, 0x00, 0x01 };
    uint8_t startCode = descriptor[3];
    if (memcmp(descriptor, startCodePrefix, 3) == 0 && (startCode >= 0x20 && startCode <= 0x2f)) {
        return true;
    }
    return false;
}

/**
 * See the VideoObjectLayer definition in the ISO/IEC 14496-2 (MPEG4 Visual)
 * specification for details for algorithm used here.
 */
void Surge::MP4VDepacketizer::ExtractDimensionsFromVolHeader(uint8_t *volHeader, size_t size) {
    SurgeUtil::BitstreamReader videoObjectLayer(volHeader, size);
    videoObjectLayer.SkipNumberOfBits(9);

    uint8_t objectLayerIdentifier = videoObjectLayer.ReadBit();
    uint32_t objectLayerVerId = 0;
    if (objectLayerIdentifier) {
        objectLayerVerId = videoObjectLayer.ReadNumberOfBits(4);
        videoObjectLayer.SkipNumberOfBits(3);
    }

    uint32_t aspectRatioInfo = videoObjectLayer.ReadNumberOfBits(4);
    if (aspectRatioInfo == 0b1111) {
        SetWidth(videoObjectLayer.ReadByte());
        SetHeight(videoObjectLayer.ReadByte());
        return;
    }

    uint8_t controlParameters = videoObjectLayer.ReadBit();
    if (controlParameters) {
        videoObjectLayer.SkipNumberOfBits(3);
        uint8_t vbvParameters = videoObjectLayer.ReadBit();
        if (vbvParameters) {
            videoObjectLayer.SkipNumberOfBits(79);
        }
    }

    uint32_t videoObjectLayerShape = videoObjectLayer.ReadNumberOfBits(2);
    if (videoObjectLayerShape == 0b00 && objectLayerVerId != 0b0001) {
        videoObjectLayer.SkipNumberOfBits(4);
    }

    videoObjectLayer.SkipNumberOfBits(1);
    uint16_t vopTimeIncrementResolution = videoObjectLayer.ReadTwoBytes();
    videoObjectLayer.SkipNumberOfBits(1);
    uint8_t fixedVopRate = videoObjectLayer.ReadBit();
    if (fixedVopRate) {
        videoObjectLayer.SkipNumberOfBits(NumberOfBitsRequiredtoStoreValue(vopTimeIncrementResolution));
    }

    if (videoObjectLayerShape != 0b10) {
        if (videoObjectLayerShape == 0) {
            videoObjectLayer.SkipNumberOfBits(1);
            SetWidth(videoObjectLayer.ReadNumberOfBits(13));
            videoObjectLayer.SkipNumberOfBits(1);
            SetHeight(videoObjectLayer.ReadNumberOfBits(13));
            return;
        }
    }
}

uint8_t Surge::MP4VDepacketizer::NumberOfBitsRequiredtoStoreValue(uint32_t value) {
    return floor(log2(value)) + 1;
}


