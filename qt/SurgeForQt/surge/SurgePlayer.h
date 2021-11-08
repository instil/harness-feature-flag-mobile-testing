#ifndef SURGEPLAYER_H
#define SURGEPLAYER_H

#include <Decoder.h>
#include <DecoderFactory.h>
#include <DateTimeConverter.h>
#include <QDateTime>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <SurgeRtspPlayer.h>
#include <SurgeVideoSurface.h>
#include <UrlProtocol.h>
#include <functional>
#include <string>

class SurgePlayer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface * videoSurface MEMBER videoSurface WRITE SetVideoSurface);

public:
    SurgePlayer(QObject *parent = nullptr);

    ~SurgePlayer();

    void InitiatePlaybackOf(const QString &url);

    void InitiatePlaybackOf(const QString &url,
                            const QString &username,
                            const QString &password);

    void InitiatePlaybackOf(const QString &url,
                            const QString &username,
                            const QString &password,
                            const QDateTime &startTime,
                            const QDateTime &endTime);

    void Play();
    void Pause();
    void Stop();

    bool IsInterleavedTcpTransport() const;

    void SetInterleavedTcpTransport(bool interleavedTcpTransport);

    void SetVideoSurface(QAbstractVideoSurface *videoSurface);

    void SetPacketBufferDelay(int bufferDelayMs);

signals:
    void StreamInitiated();
    void StreamInitiationFailed();
    void PlaybackStarted();
    void StreamStarted();
    void StreamPaused();
    void StreamStopped();
    void StreamTimeout();

private:
    void ClientReceivedFrame(const unsigned char * frameBuffer,
                             size_t length,
                             int32_t width,
                             int32_t height,
                             int32_t presentationTime,
                             int32_t duration);

private:
    void InitiatePlaybackOf(const QString &url,
                            const QString &username,
                            const QString &password,
                            const QDateTime *startTime,
                            const QDateTime *endTime);

    void AttachVideoSurface();

    QUrl CreateUrlWithUserInfo();
    QUrl GenerateMjpegPlaybackUrl();

    void DisconnectAllSignals();
    void ConnectRtspSignals();
    void ConnectHttpSignals();

private slots:
    void StateChangedOnHttpStream(QMediaPlayer::State state);
    void PlaybackErrorOnHttpStream(QMediaPlayer::Error error);
    void MediaStatusOnHttpStream(QMediaPlayer::MediaStatus status);

public:
    const Diagnostics *diagnostics;

private:
    QMediaPlayer *mediaPlayer;
    SurgeRtspPlayer *surgeRtspPlayer;
    SurgeVideoSurface *videoSurface;

    QString url;
    QString username;
    QString password;
    UrlProtocol urlProtocol;

    DiagnosticsTracker diagnosticsTracker;
};

#endif // SURGERTSPPLAYER_H
