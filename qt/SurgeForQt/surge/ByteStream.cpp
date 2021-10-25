#include "ByteStream.h"
#include <QDebug>

ByteStream::ByteStream() : QIODevice(), byteArray(new QByteArray()) { }

ByteStream::~ByteStream() {
    delete byteArray;
}

void ByteStream::close() {
    return QIODevice::close();
}

qint64 ByteStream::bytesAvailable() const {
    return byteArray->size();
}

qint64 ByteStream::readData(char *data, qint64 dataSize) {
    auto bytesToRead = dataSize < byteArray->size() ? dataSize : byteArray->size();

    memcpy(data, byteArray->data(), bytesToRead);
    byteArray->remove(0, bytesToRead);

    return bytesToRead;
}

qint64 ByteStream::writeData(const char *data, qint64 dataSize) {
    byteArray->append(data, dataSize);

    emit readyRead();

    return dataSize;
}

bool ByteStream::open(QIODevice::OpenMode mode) {
    return QIODevice::open(mode | QIODevice::Unbuffered);
}
