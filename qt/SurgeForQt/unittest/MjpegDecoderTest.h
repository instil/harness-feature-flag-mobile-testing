#ifndef MJPEGDECODERTEST_H
#define MJPEGDECODERTEST_H

#include <QtTest>
#include <QVideoWidget>
#include "MjpegDecoder.h"
#include "TestSuite.h"

class MjpegDecoderTest : public TestSuite {
    Q_OBJECT

private slots:
    void CanDecodeValidJpegFile();
    void CanNotDecodeInvalidJpegFile();
    void ShouldFirePlaybackStartedSignal();
    void ShouldFireMediaSizeChangedSignal();

private:
    QByteArray validJpeg = QByteArray::fromHex("ffd8ffdb004300030202020202030202020303030304060404040404080606050609080a0a090809090a0c0f0c0a0b0e0b09090d110d0e0f101011100a0c12131210130f101010ffc9000b080001000101011100ffcc000600101005ffda0008010100003f00d2cf20ffd9");
    QByteArray invalidJpeg = QByteArray::fromHex("00000000");

    Dispatcher dispatcher;
};

static MjpegDecoderTest TEST_MJPEGDECODER;

#endif // MJPEGDECODERTEST_H
