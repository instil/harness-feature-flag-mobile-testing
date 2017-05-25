// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

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
