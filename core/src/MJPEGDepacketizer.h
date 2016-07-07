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

#ifndef __MJPEG_DEPACKETIZER_H__
#define __MJPEG_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"
#include "Depacketizer.h"

#include <vector>

namespace Surge {

    class MJPEGDepacketizer : public Depacketizer {
    public:
        MJPEGDepacketizer(const SessionDescription sessionDescription,
                          std::vector<unsigned char> *frameBuffer)

                : Depacketizer(frameBuffer),
                  sessionDescription(sessionDescription) {}

        void ProcessPacket(const RtpPacket *packet, const bool isFirstPayload);

    private:
        
        size_t ProcessPacketHeaders(const unsigned char *payloadData, const size_t payloadLength);
        
        int ParseRestartMarkerHeader(const unsigned char *buffer);

        int ParseQuantizationTableLength(const unsigned char *buffer);
        
        void ParseQuantizationTable(const unsigned char *quantizationTableData,
                                    int quantizationTableLength,
                                    int qValue,
                                    unsigned char *lumq,
                                    unsigned char *chrq);

        const SessionDescription sessionDescription;
    };
    
};

#endif //__MJPEG_DEPACKETIZER_H__
