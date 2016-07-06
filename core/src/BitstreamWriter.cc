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

#include "BitstreamWriter.h"

SurgeUtil::BitstreamWriter::BitstreamWriter() {
    bitstream = new std::vector<uint8_t>(1);
}

SurgeUtil::BitstreamWriter::~BitstreamWriter() {
    delete bitstream;
}

void SurgeUtil::BitstreamWriter::WriteBit(uint8_t value) {
    WriteBits(value, 1);
}

void SurgeUtil::BitstreamWriter::WriteByte(uint8_t value) {
    WriteBits(value, 8);
}

void SurgeUtil::BitstreamWriter::WriteTwoBytes(uint16_t value) {
    WriteByte(value >> 8);
    WriteByte(value);
}

void SurgeUtil::BitstreamWriter::WriteThreeBytes(uint32_t value) {
    WriteByte(value >> 16);
    WriteByte(value >> 8);
    WriteByte(value);
}

void SurgeUtil::BitstreamWriter::WriteFourBytes(uint32_t value) {
    WriteByte(value >> 24);
    WriteByte(value >> 16);
    WriteByte(value >> 8);
    WriteByte(value);
}

void SurgeUtil::BitstreamWriter::WriteBytes(uint8_t *bytes, size_t size) {
    for (int i = 0; i < size; i++) {
        WriteByte(bytes[i]);
    }
}

void SurgeUtil::BitstreamWriter::WriteBits(uint8_t value, uint8_t numberOfBits) {
    uint32_t byteOffset = currentBitOffset / 8;
    uint8_t bitOffset = currentBitOffset % 8;
    uint8_t maxBitsWritable = 8 - bitOffset;
    if (numberOfBits <= maxBitsWritable) {
        bitstream->data()[byteOffset] |= value << (maxBitsWritable - numberOfBits);
    } else {
        bitstream->resize(bitstream->size() + 1);
        bitstream->data()[byteOffset] |= value >> bitOffset;
        bitstream->data()[byteOffset + 1] = value << (8 - bitOffset);
    }
    currentBitOffset += numberOfBits;
}

std::vector<uint8_t>* SurgeUtil::BitstreamWriter::Bitstream() {
    return bitstream;
}
