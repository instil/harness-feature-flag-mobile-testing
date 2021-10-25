#include "H264DecoderTest.h"
#include <QAbstractVideoSurface>
#include <QFile>

void H264DecoderTest::init() {
    videoWidget = new QVideoWidget();
    target = new H264Decoder(videoWidget->videoSurface(), &dispatcher, NULL);
}

void H264DecoderTest::cleanup() {
    delete videoWidget;
    delete target;
}

void H264DecoderTest::ShouldDecodeValidH264Stream() {
    auto bytes = ValidH264Stream();

    target->DecodeFrameBuffer(*bytes, 0, 0, 0, 0);

    QCOMPARE(videoWidget->videoSurface()->isActive(), false);
}

void H264DecoderTest::ShouldNotDecodeInvalidH264Stream() {
    auto bytes = QByteArray::fromHex("0A0B120C");

    target->DecodeFrameBuffer(bytes, 0, 0, 0, 0);

    QCOMPARE(videoWidget->videoSurface()->isActive(), false);
}

void H264DecoderTest::ShouldFirePlaybackStartedSignal() {
    QSignalSpy playbackStartedSpy(target, SIGNAL(PlaybackStarted()));
    auto bytes = ValidH264Stream();

    target->DecodeFrameBuffer(*bytes, 0, 0, 0, 0);

    QCOMPARE(playbackStartedSpy.count(), 1);
}

QByteArray *H264DecoderTest::ValidH264Stream() {
    auto path = QApplication::applicationDirPath();
    path.append("/../../SurgeForQt/unittest/data/test.264");
    auto file = new QFile(path);
    file->open(QIODevice::ReadWrite);
    auto bytes = new QByteArray();
    bytes->append(file->readAll());
    return bytes;
}
