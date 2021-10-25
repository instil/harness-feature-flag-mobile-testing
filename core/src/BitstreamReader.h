// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef BITSTREAMREADER_H
#define BITSTREAMREADER_H

#include <stdint.h>
#include <stdlib.h>

namespace SurgeUtil {

    class BitstreamReader {

    public:
        BitstreamReader(uint8_t *bitstream, size_t size);

        uint8_t ReadBit();

        uint8_t ReadByte();

        uint16_t ReadTwoBytes();

        uint32_t ReadFourBytes();

        uint32_t ReadNumberOfBits(uint8_t numberOfBits);

        void SkipNumberOfBits(uint8_t numberOfBits);

    private:
        uint8_t *bitstream;
        size_t size;
        uint32_t currentBitOffset;
    };

}


#endif //BITSTREAMREADER_H
