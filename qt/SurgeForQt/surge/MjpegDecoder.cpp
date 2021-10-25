#include "MjpegDecoder.h"
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QDebug>

MjpegDecoder::MjpegDecoder(QAbstractVideoSurface *videoSurface, Dispatcher *dispatcher, DiagnosticsTracker *diagnosticsTracker) : dispatcher(dispatcher), diagnosticsTracker(diagnosticsTracker), playbackHasStarted(false), surgeVideoSurface(nullptr) {
    SetVideoSurface(videoSurface);
}

MjpegDecoder::~MjpegDecoder() {
    if (surgeVideoSurface != nullptr) {
        surgeVideoSurface->present(QVideoFrame());
        surgeVideoSurface->deleteLater();
    }
}

void MjpegDecoder::DecodeFrameBuffer(QByteArray &frameBuffer,
                                     const int width,
                                     const int height,
                                     const int presentationTime,
                                     const int duration) {
    auto pixmap = QPixmap();
    pixmap.loadFromData(frameBuffer, "JPEG");
    auto frame = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);

    if (diagnosticsTracker != nullptr) {
        diagnosticsTracker->TrackNewFrameOfSize(frameBuffer.size() * 8);
        diagnosticsTracker->UpdateMediaSize(frame.size().width(), frame.size().height());
    }

    auto format = QVideoSurfaceFormat(frame.size(), QVideoFrame::Format_ARGB32);
    dispatcher->DispatchToThread(surgeVideoSurface, [this, &format, &frame] {
        surgeVideoSurface->start(format);
        surgeVideoSurface->present(frame);
    });

    EmitPlaybackStartedSignalIfRequired();
}

void MjpegDecoder::ChangeVideoSurface(QAbstractVideoSurface *videoSurface) {
    SetVideoSurface(videoSurface);
}

void MjpegDecoder::EmitPlaybackStartedSignalIfRequired() {
    if (!playbackHasStarted) {
        emit PlaybackStarted();
        playbackHasStarted = true;

        if (diagnosticsTracker != nullptr) {
            diagnosticsTracker->UpdateVideoFormat(VideoFormat::MJPEG);
        }
    }
}

void MjpegDecoder::SetVideoSurface(QAbstractVideoSurface *videoSurface) {
    if (videoSurface == nullptr) {
        qWarning() << "Decoder has been created without a valid video surface. Video may not play correctly until a valid video surface is provided.";
        return;
    }

    if (surgeVideoSurface != nullptr) {
        delete surgeVideoSurface;
    }

    surgeVideoSurface = new SurgeVideoSurface(videoSurface);
}
