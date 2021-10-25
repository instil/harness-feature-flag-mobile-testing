#ifndef MJPEGDECODER_H
#define MJPEGDECODER_H

#include "Decoder.h"

#include <Dispatcher.h>
#include <QPixmap>
#include <QVideoWidget>
#include <SurgeVideoSurface.h>

QT_BEGIN_NAMESPACE
class QVideoWidget;
QT_END_NAMESPACE

class MjpegDecoder : public Decoder {
    Q_OBJECT

public:
    MjpegDecoder(QAbstractVideoSurface *videoView, Dispatcher *dispatcher, DiagnosticsTracker *diagnosticsTracker);

    ~MjpegDecoder();

    void DecodeFrameBuffer(QByteArray &frameBuffer,
                           const int width,
                           const int height,
                           const int presentationTime,
                           const int duration);

    void ChangeVideoSurface(QAbstractVideoSurface *videoView);

private:
    void SetVideoSurface(QAbstractVideoSurface *videoSurface);
    void EmitPlaybackStartedSignalIfRequired();

private:
    SurgeVideoSurface *surgeVideoSurface;
    bool playbackHasStarted;

    Dispatcher *dispatcher;
    DiagnosticsTracker *diagnosticsTracker;
};

#endif // MJPEGDECODER_H
