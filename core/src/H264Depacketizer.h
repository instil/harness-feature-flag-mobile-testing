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
