#ifndef SURGEVIDEOSURFACETEST_H
#define SURGEVIDEOSURFACETEST_H

#include <QtTest>
#include <SurgeVideoSurface.h>
#include <TestSuite.h>
#include <qvideowidget.h>

class SurgeVideoSurfaceTest : public TestSuite {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void ShouldBeCreatedOnTheSameThreadAstheWrappedObject();
    void ShouldEmitFrameSizeOnFirstFrameReceived();
    void ShouldNotEmitFrameSizeOnSecondFrameReceived();

private:    
    QVideoWidget *videoWidget;
    QAbstractVideoSurface *videoSurface;
    QThread *thread;

    int width = 123;
    int height = 456;
    QVideoFrame videoFrame = QVideoFrame(nullptr, QSize(width, height), QVideoFrame::Format_RGB32);
};

static SurgeVideoSurfaceTest TEST_SURGEVIDEOSURFACETEST;

#endif // SURGEVIDEOSURFACETEST_H
