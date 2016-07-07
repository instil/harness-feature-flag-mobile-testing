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