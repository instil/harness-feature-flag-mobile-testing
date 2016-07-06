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

#ifndef DEPACKETIZER_H
#define DEPACKETIZER_H


#include <stdint.h>
#include "SessionDescription.h"
#include "RtpPacket.h"

namespace Surge {

    class Depacketizer {
    public:
        Depacketizer(std::vector<unsigned char> *frameBuffer) : frameBuffer(frameBuffer) {};

        virtual void ProcessPacket(const RtpPacket *packet, const bool isFirstPayload) = 0;

        uint32_t GetWidth() {
            return width;
        }

        uint32_t GetHeight() {
            return height;
        }

    protected:
        void AppendBytesToFrameBuffer(const unsigned char *bytes, size_t length) {
            for (int i = 0; i < length; ++i) {
                frameBuffer->push_back(bytes[i]);
            }
        }

        void SetWidth(uint32_t width) {
            this->width = width;
        }

        void SetHeight(uint32_t height) {
            this->height = height;
        }

        std::vector<unsigned char> *frameBuffer;
        uint32_t width;
        uint32_t height;
    };

}

#endif //DEPACKETIZER_H
