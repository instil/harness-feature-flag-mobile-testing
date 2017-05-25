// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include <math.h>
#include "BitstreamReader.h"

SurgeUtil::BitstreamReader::BitstreamReader(uint8_t *bitstream, size_t size) {
    this->bitstream = bitstream;
    this->size = size;
    this->currentBitOffset = 0;
}

uint8_t SurgeUtil::BitstreamReader::ReadBit() {
    return ReadNumberOfBits(1);
}

uint8_t SurgeUtil::BitstreamReader::ReadByte() {
    return ReadNumberOfBits(8);
}

uint16_t SurgeUtil::BitstreamReader::ReadTwoBytes() {
    return ReadNumberOfBits(16);
}

uint32_t SurgeUtil::BitstreamReader::ReadFourBytes() {
    return ReadNumberOfBits(32);
}

uint32_t SurgeUtil::BitstreamReader::ReadNumberOfBits(uint8_t numberOfBits) {
    uint32_t byteOffset = currentBitOffset / 8;
    uint8_t bitOffset = currentBitOffset % 8;
    uint8_t maxBitsReadable = 8 - bitOffset;
    uint8_t bitsToRead = numberOfBits <= maxBitsReadable ? numberOfBits : maxBitsReadable;

    uint8_t mask = pow(2, bitsToRead) - 1;
    uint32_t value = (bitstream[byteOffset] >> (8 - bitsToRead - bitOffset)) & mask;
    currentBitOffset += bitsToRead;

    uint8_t bitsRemaining = numberOfBits - bitsToRead;
    if (bitsRemaining > 0) {
        value = (value << bitsRemaining) | ReadNumberOfBits(bitsRemaining);
    }

    return value;
}

void SurgeUtil::BitstreamReader::SkipNumberOfBits(uint8_t numberOfBits) {
    currentBitOffset += numberOfBits;
}
