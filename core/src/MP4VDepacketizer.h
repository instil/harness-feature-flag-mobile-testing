// -*-c++-*-
// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
