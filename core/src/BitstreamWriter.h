// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef BITSTREAMWRITER_H
#define BITSTREAMWRITER_H

#include <stdint.h>
#include <stdlib.h>
#include <vector>

namespace SurgeUtil {

    class BitstreamWriter {
    public:
        BitstreamWriter();

        ~BitstreamWriter();

        void WriteBit(uint8_t value);

        void WriteByte(uint8_t value);

        void WriteTwoBytes(uint16_t value);

        void WriteThreeBytes(uint32_t value);

        void WriteFourBytes(uint32_t value);

        void WriteBytes(uint8_t *bytes, size_t size);

        void WriteBits(uint8_t value, uint8_t numberOfBits);

        std::vector<uint8_t>* Bitstream();

    private:
        std::vector<uint8_t> *bitstream;
        uint32_t currentBitOffset;
    };

}

#endif //BITSTREAMWRITER_H
