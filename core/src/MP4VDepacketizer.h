// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __MP4V_DEPACKETIZER_H__
#define __MP4V_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"
#include "Depacketizer.h"

#include <vector>

namespace Surge {

    class MP4VDepacketizer : public Depacketizer {
    public:
        MP4VDepacketizer(const SessionDescription sessionDescription,
                         std::vector<unsigned char> *frameBuffer)

                : Depacketizer(frameBuffer),
                  sessionDescription(sessionDescription) {}

        void ProcessPacket(const RtpPacket *packet, const bool isFirstPayload);

    private:
        void ExtractDimensionsFromVosHeader(std::vector<unsigned char> config);

        bool IsVideoObjectLayerStartCode(uint8_t *descriptor);

        void ExtractDimensionsFromVolHeader(uint8_t *volHeader, size_t size);

        uint8_t NumberOfBitsRequiredtoStoreValue(uint32_t value);

        const SessionDescription sessionDescription;
    };
    
};

#endif //__MP4V_DEPACKETIZER_H__
