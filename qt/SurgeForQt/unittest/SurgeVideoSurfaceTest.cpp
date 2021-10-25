#include "SurgeVideoSurfaceTest.h"

#include <QThread>

void SurgeVideoSurfaceTest::init() {
    thread = QThread::create([this] {
        videoWidget = new QVideoWidget();
        videoSurface = videoWidget->videoSurface();
    });

    thread->start();
    thread->wait();
}

void SurgeVideoSurfaceTest::cleanup() {
    videoWidget->deleteLater();

    thread->quit();
    thread->wait();
    thread->deleteLater();
}

void SurgeVideoSurfaceTest::ShouldBeCreatedOnTheSameThreadAstheWrappedObject() {
    SurgeVideoSurface target(videoSurface);

    QCOMPARE(target.thread(), videoSurface->thread());
}

void SurgeVideoSurfaceTest::ShouldEmitFrameSizeOnFirstFrameReceived() {
    SurgeVideoSurface target(videoSurface);
    QSignalSpy frameSizeSpy(&target, &SurgeVideoSurface::FrameSize);

    target.present(videoFrame);

    QCOMPARE(frameSizeSpy.size(), 1);
    auto arguments = frameSizeSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), width);
    QCOMPARE(arguments.at(1).toInt(), height);
}


void SurgeVideoSurfaceTest::ShouldNotEmitFrameSizeOnSecondFrameReceived() {
    SurgeVideoSurface target(videoSurface);
    QSignalSpy frameSizeSpy(&target, &SurgeVideoSurface::FrameSize);

    target.present(videoFrame);
    target.present(videoFrame);

    QCOMPARE(frameSizeSpy.size(), 1);
}
