// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __H264_DEPACKETIZER_H__
#define __H264_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"
#include "Depacketizer.h"

#include <vector>

namespace Surge {

    class H264Depacketizer : public Depacketizer {
    public:
        H264Depacketizer(const SessionDescription sessionDescription,
                         std::vector<unsigned char> *frameBuffer)

                : Depacketizer(frameBuffer),
                  sessionDescription(sessionDescription) {}

        void ProcessPacket(const RtpPacket *packet, const bool isFirstPayload);

    private:
        int GetH264NaluTypeFromByte(const unsigned char byte) const {
            return byte & 0x1F;
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

#endif //__H264_DEPACKETIZER_H__
