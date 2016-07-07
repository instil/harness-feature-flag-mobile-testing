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

#ifndef BITSTREAMWRITER_H
#define BITSTREAMWRITER_H


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
