#include "H265DecoderTest.h"
#include <QAbstractVideoSurface>
#include <QFile>

void H265DecoderTest::init() {
    videoWidget = new QVideoWidget();
    target = new H265Decoder(videoWidget->videoSurface(), &dispatcher, NULL);
}

void H265DecoderTest::cleanup() {
    delete videoWidget;
    delete target;
}

void H265DecoderTest::ShouldDecodeValidH265Stream() {
    auto bytes = ValidH265Stream();

    target->DecodeFrameBuffer(*bytes, 0, 0, 0, 0);
}

void H265DecoderTest::ShouldNotDecodeInvalidH265Stream() {
    auto bytes = QByteArray::fromHex("0A0B120C");

    target->DecodeFrameBuffer(bytes, 0, 0, 0, 0);
}

void H265DecoderTest::ShouldFirePlaybackStartedSignal() {
    QSignalSpy playbackStartedSpy(target, SIGNAL(PlaybackStarted()));
    auto bytes = ValidH265Stream();

    target->DecodeFrameBuffer(*bytes, 0, 0, 0, 0);

    QCOMPARE(playbackStartedSpy.count(), 1);
}

QByteArray *H265DecoderTest::ValidH265Stream() {
    auto path = QApplication::applicationDirPath();
    path.append("/../../SurgeForQt/unittest/data/test.265");
    auto file = new QFile(path);
    file->open(QIODevice::ReadWrite);
    auto bytes = new QByteArray();
    bytes->append(file->readAll());
    return bytes;
}
