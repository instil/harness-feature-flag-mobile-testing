#ifndef NALUMAGICBYTEHEADER_H
#define NALUMAGICBYTEHEADER_H

struct NaluMagicByteHeader {
    const int position;
    const int length;

    NaluMagicByteHeader(int position, int length) : position(position), length(length) {}
};

#endif // NALUMAGICBYTEHEADER_H
