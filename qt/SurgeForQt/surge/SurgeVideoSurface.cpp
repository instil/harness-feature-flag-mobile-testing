#include "SurgeVideoSurface.h"

#include <QVideoSurfaceFormat>
#include <QDebug>

SurgeVideoSurface::SurgeVideoSurface(QAbstractVideoSurface *videoSurface) : videoSurface(videoSurface), firstFrame(true), surfaceIsActive(true) {
    if (videoSurface == nullptr) {
        qWarning() << "VideoSurface provided is null. Using this surface will cause an error.";
    } else {
        if (thread() != videoSurface->thread()) {
            moveToThread(videoSurface->thread());
        }

        connect(videoSurface, &QAbstractVideoSurface::destroyed, this, &SurgeVideoSurface::Destroyed);
    }
}

bool SurgeVideoSurface::present(const QVideoFrame &frame) {
    if (!surfaceIsActive) {
        return false;
    }

    if (firstFrame) {
        auto size = frame.size();
        emit FrameSize(size.width(), size.height());
        firstFrame = false;
    }

    emit FrameBitrate(frame.mappedBytes());
    return videoSurface->present(frame);
}

bool SurgeVideoSurface::isFormatSupported(const QVideoSurfaceFormat &format) const {
    if (!surfaceIsActive) {
        return false;
    }

    return videoSurface->isFormatSupported(format);
}

QVideoSurfaceFormat SurgeVideoSurface::nearestFormat(const QVideoSurfaceFormat &format) const {
    if (!surfaceIsActive) {
        return QVideoSurfaceFormat();
    }

    return videoSurface->nearestFormat(format);
}

bool SurgeVideoSurface::start(const QVideoSurfaceFormat &format) {
    if (!surfaceIsActive) {
        return false;
    }

    return videoSurface->start(format);
}

void SurgeVideoSurface::stop() {
    if (!surfaceIsActive) {
        return;
    }

    videoSurface->stop();
}

QList<QVideoFrame::PixelFormat> SurgeVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const {
    if (!surfaceIsActive) {
        return QList<QVideoFrame::PixelFormat>();
    }

    return videoSurface->supportedPixelFormats(type);
}

void SurgeVideoSurface::Destroyed() {
    surfaceIsActive = false;
}
