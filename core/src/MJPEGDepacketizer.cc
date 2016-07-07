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

#include "MJPEGDepacketizer.h"

#include <cstdlib>

#define JPEGHEADERSIZE 8
#define RESTARTMARKERHEADERSIZE 4
#define QUANTIZATIONTABLEHEADERSIZE 4
#define RESTART_MIN 64
#define RESTART_MAX 127

// https://tools.ietf.org/html/rfc2435
/*
 * Table K.1 from JPEG spec.
 */
static const int jpeg_luma_quantizer[64] = {
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 113, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99
};

/*
 * Table K.2 from JPEG spec.
 */
static const int jpeg_chroma_quantizer[64] = {
        17, 18, 24, 47, 99, 99, 99, 99,
        18, 21, 26, 66, 99, 99, 99, 99,
        24, 26, 56, 99, 99, 99, 99, 99,
        47, 66, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99
};

static unsigned char lum_dc_codelens[] = {
    0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
};

static unsigned char lum_dc_symbols[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static unsigned char lum_ac_codelens[] = {
    0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d,
};

static unsigned char lum_ac_symbols[] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};

static unsigned char chm_dc_codelens[] = {
    0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

static unsigned char chm_dc_symbols[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static unsigned char chm_ac_codelens[] = {
    0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77,
};

static unsigned char chm_ac_symbols[] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};

/*
 * Call MakeTables with the Q factor and two u_char[64] return arrays
 */
static
void MakeTables(int q, unsigned char *lqt, unsigned char *cqt) {
    int i;
    int factor = q;

    if (q < 1) factor = 1;
    if (q > 99) factor = 99;
    if (q < 50)
        q = 5000 / factor;
    else
        q = 200 - factor*2;

    for (i = 0; i < 64; i++) {
        int lq = (jpeg_luma_quantizer[i] * q + 50) / 100;
        int cq = (jpeg_chroma_quantizer[i] * q + 50) / 100;

        /* Limit the quantizers to 1 <= q <= 255 */
        if (lq < 1) lq = 1;
        else if (lq > 255) lq = 255;
        lqt[i] = lq;

        if (cq < 1) cq = 1;
        else if (cq > 255) cq = 255;
        cqt[i] = cq;
    }
}

static
void MakeQuantHeader(std::vector<unsigned char> * const p, unsigned char *qt, int tableNo) {
    p->push_back(0xff);
    p->push_back(0xdb);
    p->push_back(0);
    p->push_back(67);
    p->push_back(tableNo);

    size_t preCopySize = p->size();
    p->resize(p->size() + 64);
    std::copy(qt, qt + 64, p->begin() + preCopySize);
}

static
void MakeHuffmanHeader(std::vector<unsigned char> * const p,
                       unsigned char *codelens,
                       int ncodes,
                       unsigned char *symbols,
                       int nsymbols,
                       int tableNo,
                       int tableClass) {

    p->push_back(0xff);
    p->push_back(0xc4);
    p->push_back(0);
    p->push_back(3 + ncodes + nsymbols);
    p->push_back((tableClass << 4) | tableNo);

    size_t preCopySize = p->size();

    p->resize(p->size() + ncodes + nsymbols);

    std::copy(codelens, codelens + ncodes, p->begin() + preCopySize);
    std::copy(symbols, symbols + nsymbols, p->begin() + preCopySize + ncodes);
}

static
void MakeDRIHeader(std::vector<unsigned char> * const p, unsigned short dri) {
    p->push_back(0xff);
    p->push_back(0xdd);
    p->push_back(0x0);
    p->push_back(4);
    p->push_back(dri >> 8);
    p->push_back(dri & 0xff);
}

/*
 *  Arguments:
 *    type, width, height: as supplied in RTP/JPEG header
 *    lqt, cqt: quantization tables as either derived from
 *         the Q field using MakeTables() or as specified
 *         in section 4.2.
 *    dri: restart interval in MCUs, or 0 if no restarts.
 *
 *    p: pointer to return area
 *
 *  Return value:
 *    The length of the generated headers.
 *
 *    Generate a frame and scan headers that can be prepended to the
 *    RTP/JPEG data payload to produce a JPEG compressed image in
 *    interchange format (except for possible trailing garbage and
 *    absence of an EOI marker to terminate the scan).
 */
static
void MakeHeaders(std::vector<unsigned char> * const p,
                 int type,
                 int w,
                 int h,
                 unsigned char *lqt,
                 unsigned char *cqt,
                 unsigned short dri) {

    /* convert from blocks to pixels */
    w <<= 3;
    h <<= 3;

    p->push_back(0xff);
    p->push_back(0xd8);

    MakeQuantHeader(p, lqt, 0);
    MakeQuantHeader(p, cqt, 1);

    if (dri != 0)
        MakeDRIHeader(p, dri);

    p->push_back(0xff);
    p->push_back(0xc0);
    p->push_back(0);
    p->push_back(17);
    p->push_back(8);
    p->push_back(h >> 8);
    p->push_back(h);
    p->push_back(w >> 8);
    p->push_back(w);
    p->push_back(3);
    p->push_back(0);
    
    if (type == 0)
        p->push_back(0x21);
    else
        p->push_back(0x22);

    p->push_back(0);
    p->push_back(1);
    p->push_back(0x11);
    p->push_back(1);
    p->push_back(2);
    p->push_back(0x11);
    p->push_back(1);
    
    MakeHuffmanHeader(p, lum_dc_codelens,
                      sizeof(lum_dc_codelens),
                      lum_dc_symbols,
                      sizeof(lum_dc_symbols), 0, 0);
    MakeHuffmanHeader(p, lum_ac_codelens,
                      sizeof(lum_ac_codelens),
                      lum_ac_symbols,
                      sizeof(lum_ac_symbols), 0, 1);
    MakeHuffmanHeader(p, chm_dc_codelens,
                      sizeof(chm_dc_codelens),
                      chm_dc_symbols,
                      sizeof(chm_dc_symbols), 1, 0);
    MakeHuffmanHeader(p, chm_ac_codelens,
                      sizeof(chm_ac_codelens),
                      chm_ac_symbols,
                      sizeof(chm_ac_symbols), 1, 1);

    p->push_back(0xff);
    p->push_back(0xda);
    p->push_back(0);
    p->push_back(12);
    p->push_back(3);
    p->push_back(0);
    p->push_back(0);
    p->push_back(1);
    p->push_back(0x11);
    p->push_back(2);
    p->push_back(0x11);
    p->push_back(0);
    p->push_back(63);
    p->push_back(0);
}

void Surge::MJPEGDepacketizer::ProcessPacket(const RtpPacket *packet, const bool isFirstPayload) {
    const unsigned char *payloadData = packet->PayloadData();
    const size_t payloadLength = packet->PayloadLength();

    size_t jpegHeaderSize = ProcessJpegHeaders(packet);

//    size_t frame_length = frameBuffer->size();
//
//    if (frame_length == 0 && m_fragmentOffset > 0) {
//        return;
//    }

    for (size_t i = jpegHeaderSize; i < payloadLength; i++) {
        frameBuffer->push_back(payloadData[i]);
    }
}

//void Surge::MJPEGDepacketizer::

size_t Surge::MJPEGDepacketizer::ProcessJpegHeaders(const RtpPacket *packet) {
    const unsigned char *payloadData = packet->PayloadData();
    const size_t payloadLength = packet->PayloadLength();

    const unsigned char * fragmentOffset = &payloadData[1];
    const unsigned char * typeOffset =  &payloadData[4];
    const unsigned char * qOffset = &payloadData[5];
    const unsigned char * widthOffset = &payloadData[6];
    const unsigned char * heightOffset = &payloadData[7];

    // reset variables
    int m_fragmentOffset = 0;
    int m_type = 0;
    int m_qValue = 0;
    int m_width = 0;
    int m_height = 0;

    m_fragmentOffset |= 0x00 & 0xFF;
    m_fragmentOffset <<= 8;
    m_fragmentOffset |= fragmentOffset[0] & 0xFF;
    m_fragmentOffset <<= 8;
    m_fragmentOffset |= fragmentOffset[1] & 0xFF;
    m_fragmentOffset <<= 8;
    m_fragmentOffset |= fragmentOffset[2] & 0xFF;

    m_type |= typeOffset[0] & 0xFF;
    m_qValue |= qOffset[0] & 0xFF;
    m_width |= widthOffset[0] & 0xFF;
    m_height |= heightOffset[0] & 0xFF;
//    ParseJpegHeader(payloadData);

    int restartHeaderSize = 0;
    int quantizationPayloadLength = 0;

    if (m_fragmentOffset == 0) {
        int dri = 0;
        restartHeaderSize = ((m_type >= RESTART_MIN) && (m_type <= RESTART_MAX))
                            ? RESTARTMARKERHEADERSIZE : 0;
        if (restartHeaderSize > 0) {
            dri = ParseRestartMarkerHeader(payloadData + JPEGHEADERSIZE);
        }

//        ParseQuantizationHeader(payloadData + JPEGHEADERSIZE + restartHeaderSize);
        unsigned char m_lumq[64];
        unsigned char m_chrq[64];
        if (m_fragmentOffset == 0) {
            const unsigned char *quantizationHeader =
                    payloadData + JPEGHEADERSIZE + restartHeaderSize;
            quantizationPayloadLength |= quantizationHeader[2] & 0xFF;
            quantizationPayloadLength <<= 8;
            quantizationPayloadLength |= quantizationHeader[3] & 0xFF;
        }

//        if (quantizationPayloadLength > 0) {
//            ParseQuantizationTableData(payloadData
//                                       + JPEGHEADERSIZE
//                                       + restartHeaderSize
//                                       + QUANTIZATIONTABLEHEADERSIZE);
//        }

        if (quantizationPayloadLength > 0) {
            int quantizationTableDataPosition = 0;
            size_t lumqPosition;
            const unsigned char *quantizationTableData = payloadData
                                                         + JPEGHEADERSIZE
                                                         + restartHeaderSize
                                                         + QUANTIZATIONTABLEHEADERSIZE;
            for (lumqPosition = 0; lumqPosition < 64 && lumqPosition <
                                                        quantizationPayloadLength; lumqPosition++) {
                m_lumq[lumqPosition] = quantizationTableData[quantizationTableDataPosition];
                quantizationTableDataPosition++;
            }

            int chrqPosition;
            for (chrqPosition = 0; chrqPosition < 64 && chrqPosition <
                                                        quantizationPayloadLength; chrqPosition++) {
                m_chrq[chrqPosition] = quantizationTableData[quantizationTableDataPosition];
                quantizationTableDataPosition++;
            }
        } else {
            // create tables from qfactor
            MakeTables(m_qValue, m_lumq, m_chrq);
        }

        MakeHeaders(frameBuffer, m_type, m_width, m_height, m_lumq, m_chrq, dri);
    }

    size_t jpeg_payload_offset = JPEGHEADERSIZE;
    if (m_fragmentOffset == 0) {
        jpeg_payload_offset += (restartHeaderSize
                                + QUANTIZATIONTABLEHEADERSIZE
                                + quantizationPayloadLength);
    }

    if (m_fragmentOffset != 0) {
        jpeg_payload_offset += restartHeaderSize;
    }

    return jpeg_payload_offset;
}

void Surge::MJPEGDepacketizer::ParseJpegHeader(const unsigned char *buffer) {
    
//    const unsigned char * fragmentOffset = &buffer[1];
//    const unsigned char * typeOffset =  &buffer[4];
//    const unsigned char * qOffset = &buffer[5];
//    const unsigned char * widthOffset = &buffer[6];
//    const unsigned char * heightOffset = &buffer[7];
//
//    // reset variables
//    m_fragmentOffset = 0;
//    m_type = 0;
//    m_qValue = 0;
//    m_width = 0;
//    m_height = 0;
//
//    m_fragmentOffset |= 0x00 & 0xFF;
//    m_fragmentOffset <<= 8;
//    m_fragmentOffset |= fragmentOffset[0] & 0xFF;
//    m_fragmentOffset <<= 8;
//    m_fragmentOffset |= fragmentOffset[1] & 0xFF;
//    m_fragmentOffset <<= 8;
//    m_fragmentOffset |= fragmentOffset[2] & 0xFF;
//
//    m_type |= typeOffset[0] & 0xFF;
//    m_qValue |= qOffset[0] & 0xFF;
//    m_width |= widthOffset[0] & 0xFF;
//    m_height |= heightOffset[0] & 0xFF;
}

int Surge::MJPEGDepacketizer::ParseRestartMarkerHeader(const unsigned char * buffer) {
    int dri = 0;
    dri = buffer[0];
    dri = dri << 8;
    dri = dri | buffer[1];
    return dri;
}

void Surge::MJPEGDepacketizer::ParseQuantizationHeader(const unsigned char * quantizationHeader) {
//    quantizationPayloadLength = 0;
//    if (m_fragmentOffset == 0) {
//        quantizationPayloadLength |= quantizationHeader[2] & 0xFF;
//        quantizationPayloadLength <<= 8;
//        quantizationPayloadLength |= quantizationHeader[3] & 0xFF;
//    }
}

void Surge::MJPEGDepacketizer::ParseQuantizationTableData(const unsigned char * quantizationTableData) {
//    if (quantizationPayloadLength > 0) {
//        int quantizationTableDataPosition = 0;
//        size_t lumqPosition;
//        for (lumqPosition = 0; lumqPosition < 64 && lumqPosition <
//                                                    quantizationPayloadLength; lumqPosition++) {
//            m_lumq[lumqPosition] = quantizationTableData[quantizationTableDataPosition];
//            quantizationTableDataPosition++;
//        }
//
//        int chrqPosition;
//        for (chrqPosition = 0; chrqPosition < 64 && chrqPosition <
//                                                    quantizationPayloadLength; chrqPosition++) {
//            m_chrq[chrqPosition] = quantizationTableData[quantizationTableDataPosition];
//            quantizationTableDataPosition++;
//        }
//    } else {
//        // create tables from qfactor
//        MakeTables(m_qValue, m_lumq, m_chrq);
//    }
}
