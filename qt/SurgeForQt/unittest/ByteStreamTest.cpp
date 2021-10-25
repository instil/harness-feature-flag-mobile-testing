#include "ByteStreamTest.h"
#include <QBuffer>

void ByteStreamTest::ShouldReadFromByteStream() {
    auto byteStream = new ByteStream();
    byteStream->open(QIODevice::ReadWrite);
    byteStream->write("first");
    auto first = byteStream->readAll();
    auto actual = first.data();
    QCOMPARE(actual, "first");
}

void ByteStreamTest::ShouldReadMutlipleTimesFromByteStream() {
    auto byteStream = new ByteStream();
    byteStream->open(QIODevice::ReadWrite);
    byteStream->write("first");
    auto first = byteStream->readAll();
    auto actual = first.data();
    QCOMPARE(actual, "first");

    byteStream->write("second");
    auto second = byteStream->readAll();
    actual = second.data();
    QCOMPARE(actual, "second");
}
