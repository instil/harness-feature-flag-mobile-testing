#ifndef SURGERTSPPLAYER_H
#define SURGERTSPPLAYER_H

#include <Decoder.h>
#include <DecoderFactory.h>
#include <DateTimeConverter.h>
#include <QDateTime>
#include <QVideoWidget>
#include <UrlProtocol.h>
#include <functional>
#include <string>
#include <SurgeCoreDeclarations.h>

class SurgeRtspPlayer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface * videoSurface MEMBER videoSurface WRITE SetVideoSurface);
    Q_PROPERTY(bool interleavedTcpTransport READ IsInterleavedTcpTransport WRITE SetInterleavedTcpTransport);
    Q_PROPERTY(bool tlsCertificateValidationEnabled READ IsTlsCertificateValidationEnabled WRITE SetTlsCertificateValidationEnabled);
    Q_PROPERTY(bool tlsSelfSignedCertificateAllowed READ IsTlsSelfSignedCertificateAllowed WRITE SetTlsSelfSignedCertificateAllowed);

    class InternalSurgeDelegateWrapper;
    class InternalSurgeCoreLogger;

    friend InternalSurgeDelegateWrapper;

public:
    SurgeRtspPlayer(QObject *parent = nullptr);

    ~SurgeRtspPlayer();

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
    void Seek(QDateTime &startTime, QDateTime &endTime);
    bool IsInterleavedTcpTransport() const;
    void SetInterleavedTcpTransport(bool interleavedTcpTransport);

    bool IsTlsCertificateValidationEnabled() const;

    void SetTlsCertificateValidationEnabled(bool tlsCertificateValidationEnabled);

    bool IsTlsSelfSignedCertificateAllowed() const;

    void SetTlsSelfSignedCertificateAllowed(bool tlsSelfSignedCertificateAllowed);

    void SetTlsTrustedCertificatePath(QString filePath);

    void SetVideoSurface(QAbstractVideoSurface *videoSurface);

    void SetPacketBufferDelay(int packetBufferDelayMs);

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

    bool HandleAnyInitializationErrors(const Surge::RtspResponse *response);

    void SetCredentials(const std::string &username,
                        const std::string &password);

    void SetTimeRange(const QDateTime *startTime, const QDateTime *endTime);

    void HandleStreamTimeout();

    Surge::SessionDescription SelectPreferredSessionDescription(const std::vector<Surge::SessionDescription> &sessionDescriptions);

    void InitializeDecoder(Surge::SessionDescription &sessionDescription);
    void ResetPlayer();

public:
    const Diagnostics *diagnostics;

private:
    InternalSurgeDelegateWrapper *surgeCoreDelegate;

    Surge::RtspClient *rtspClient;
    Decoder *decoder;
    QAbstractVideoSurface *videoSurface;

    QString url;
    QString username;
    QString password;
    UrlProtocol urlProtocol;

    DecoderFactory decoderFactory;
    Dispatcher dispatcher;
    DiagnosticsTracker *diagnosticsTracker;
};

#endif // SURGERTSPPLAYER_H
