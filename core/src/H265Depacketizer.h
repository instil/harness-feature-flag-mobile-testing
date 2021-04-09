// Copyright (c) 2020 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef H265Depacketizer_h
#define H265Depacketizer_h

#include "SessionDescription.h"
#include "RtpPacket.h"
#include "Depacketizer.h"

#include <vector>

namespace Surge {

    class H265Depacketizer : public Depacketizer {
    public:
        H265Depacketizer(const SessionDescription sessionDescription,
                         std::vector<unsigned char> *frameBuffer)
                : Depacketizer(frameBuffer),
                  sessionDescription(sessionDescription) {}

        void ProcessPacket(const RtpPacket *packet, const bool isFirstPayload);

    private:
        const int VPS = 32;
        const int SPS = 33;
        const int PPS = 34;

        int GetH265NaluTypeFromByte(const unsigned char byte) const {
            return (byte & 0x7E) >> 1;
        }

        void HandleFragmentationPacket(const RtpPacket *packet);
        unsigned char * ConstructNalUnitHeaderFromFragmentationPacket(const RtpPacket *packet);
        void AppendBase64EncodedNalUnit(std::string base64NalUnit);

        void AppendNaluHeaderToFrame(int h265NaluType) {
            if (h265NaluType == VPS || h265NaluType == SPS || h265NaluType == PPS) {
                AppendFourByteNaluHeaderToFrame();
            } else {
                AppendThreeByteNaluHeaderToFrame();
            }
        }

        void AppendFourByteNaluHeaderToFrame() {
            unsigned char naluHeader[] = { 0x00, 0x00, 0x00, 0x01 };
            AppendBytesToFrameBuffer(naluHeader, sizeof(naluHeader));
        }

        void AppendThreeByteNaluHeaderToFrame() {
            unsigned char naluHeader[] = { 0x00, 0x00, 0x01 };
            AppendBytesToFrameBuffer(naluHeader, sizeof(naluHeader));
        }

        const SessionDescription sessionDescription;

    };

};

#endif /* H265Depacketizer_h */
