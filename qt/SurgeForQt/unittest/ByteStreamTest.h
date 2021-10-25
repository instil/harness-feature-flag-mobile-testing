#ifndef BYTESTREAMTEST_H
#define BYTESTREAMTEST_H

#include <QtTest>
#include <ByteStream.h>
#include "TestSuite.h"

class ByteStreamTest : public TestSuite {
    Q_OBJECT

private slots:
    void ShouldReadFromByteStream();
    void ShouldReadMutlipleTimesFromByteStream();
};

static ByteStreamTest TEST_BYTESTREAM;

#endif // BYTESTREAMTEST_H

