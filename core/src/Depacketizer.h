// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

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
