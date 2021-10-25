#ifndef DECODER_H
#define DECODER_H

#include <DiagnosticsTracker.h>
#include <QAbstractVideoSurface>
#include <QByteArray>

class Decoder : public QObject {
    Q_OBJECT

public:
    Decoder() { }

    virtual ~Decoder() { }

    virtual void DecodeFrameBuffer(QByteArray &frameBuffer,
                                   const int width,
                                   const int height,
                                   const int presentationTime,
                                   const int duration) = 0;

    virtual void ChangeVideoSurface(QAbstractVideoSurface *videoSurface) = 0;

signals:
    void PlaybackStarted();
};

#endif // DECODER_H
