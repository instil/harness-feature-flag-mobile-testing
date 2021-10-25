#include "MjpegDecoderTest.h"
#include <QAbstractVideoSurface>
#include <QThread>

void MjpegDecoderTest::CanDecodeValidJpegFile() {
    auto videoView = new QVideoWidget();
    auto videoSurface = videoView->videoSurface();
    auto mjpegDecoder = new MjpegDecoder(videoSurface, &dispatcher, NULL);

    auto thread = QThread::create([this, videoSurface, mjpegDecoder] {
        mjpegDecoder->DecodeFrameBuffer(validJpeg, 1, 1, 0, 0);

        QCOMPARE(videoSurface->error(), QAbstractVideoSurface::NoError);
    });
    thread->start();
}

void MjpegDecoderTest::CanNotDecodeInvalidJpegFile() {
    auto videoView = new QVideoWidget();
    auto videoSurface = videoView->videoSurface();
    auto mjpegDecoder = new MjpegDecoder(videoSurface, &dispatcher, NULL);

    auto thread = QThread::create([this, videoSurface, mjpegDecoder] {
        mjpegDecoder->DecodeFrameBuffer(invalidJpeg, 1, 1, 0, 0);

        QCOMPARE(videoSurface->error(), QAbstractVideoSurface::StoppedError);
    });
    thread->start();
}

void MjpegDecoderTest::ShouldFirePlaybackStartedSignal() {
    auto videoView = new QVideoWidget();
    auto videoSurface = videoView->videoSurface();
    auto mjpegDecoder = new MjpegDecoder(videoSurface, &dispatcher, NULL);

    auto thread = QThread::create([this, mjpegDecoder] {
        QSignalSpy playbackStartedSpy(mjpegDecoder, SIGNAL(PlaybackStarted()));

        mjpegDecoder->DecodeFrameBuffer(invalidJpeg, 1, 1, 0, 0);

        QCOMPARE(playbackStartedSpy.count(), 1);
    });
    thread->start();
}

void MjpegDecoderTest::ShouldFireMediaSizeChangedSignal() {
    auto videoView = new QVideoWidget();
    auto videoSurface = videoView->videoSurface();
    auto diagnosticsTracker = new DiagnosticsTracker(nullptr);
    auto mjpegDecoder = new MjpegDecoder(videoSurface, &dispatcher, diagnosticsTracker);

    auto thread = QThread::create([this, mjpegDecoder, diagnosticsTracker] {
        QSignalSpy mediaSizeChangedSpy(diagnosticsTracker, SIGNAL(MediaSizeChanged(int, int)));

        mjpegDecoder->DecodeFrameBuffer(validJpeg, 1, 1, 0, 0);

        QCOMPARE(mediaSizeChangedSpy.count(), 1);
    });
    thread->start();
}
