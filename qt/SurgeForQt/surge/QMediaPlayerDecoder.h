#ifndef QMEDIAPLAYERDECODER_H
#define QMEDIAPLAYERDECODER_H

#include <ByteStream.h>
#include <Decoder.h>
#include <Dispatcher.h>
#include <QByteArray>
#include <QMediaPlayer>
#include <QUrl>
#include <SurgeVideoSurface.h>

QT_BEGIN_NAMESPACE
class QMediaPlayer;
QT_END_NAMESPACE

class QMediaPlayerDecoder : public Decoder {
public:
    QMediaPlayerDecoder(QAbstractVideoSurface *videoSurface, Dispatcher *dispatcher, DiagnosticsTracker *diagnosticsTracker);

    ~QMediaPlayerDecoder();

    void DecodeFrameBuffer(QByteArray &frameBuffer,
                           const int width,
                           const int height,
                           const int presentationTime,
                           const int duration);

    void ChangeVideoSurface(QAbstractVideoSurface *videoSurface);

private:
    virtual QUrl GstreamerPipeline() { return QUrl(); }

protected:
    virtual VideoFormat GetDecoderFormat() = 0;

private:
    void StartDecoder();
    bool DecoderNeedsToBeStarted();
    void SetVideoSurface(QAbstractVideoSurface *videoSurface);
    void DeleteDecoder();

private:
    QMediaPlayer *player;
    SurgeVideoSurface *surgeVideoSurface;
    QIODevice *stream;
    bool decoderIsRunning;

    DiagnosticsTracker *diagnosticsTracker;
    Dispatcher *dispatcher;
};

#endif // QMEDIAPLAYERDECODER_H
