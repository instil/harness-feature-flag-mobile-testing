#include "SurgeRtspPlayer.h"

#include <MjpegDecoder.h>
#include <RtspClient.h>
#include <RtspErrorCode.h>
#include <QDebug>

class SurgeRtspPlayer::InternalSurgeDelegateWrapper : public Surge::IRtspClientDelegate {

public:
    InternalSurgeDelegateWrapper(SurgeRtspPlayer *player) : player(player) { }

    void ClientDidTimeout() {
        player->HandleStreamTimeout();
    }

    void ClientReceivedFrame(const unsigned char * frameBuffer,
                                     size_t length,
                                     int32_t width,
                                     int32_t height,
                                     int32_t presentationTime,
                                     int32_t duration) {
        player->ClientReceivedFrame(frameBuffer, length, width, height, presentationTime, duration);
    }

    void StreamConfigChanged(bool wasRedirect) { }

    void ClientReceivedExtendedHeader(const unsigned char * buffer, size_t length) { }

private:
    SurgeRtspPlayer *player;
};

class SurgeRtspPlayer::InternalSurgeCoreLogger : public SurgeUtil::ILoggingDelegate {

public:
    InternalSurgeCoreLogger() { }

    virtual void info(const char *message) const { qInfo() << message; }

    virtual void error(const char *message) const { qCritical() << message; }

    virtual void debug(const char *message) const { qDebug() << message; }

    virtual void fatal(const char *message) const { qFatal("%s", message); }

    virtual void warn(const char *message) const { qWarning() << message; }

    virtual void trace(const char *message) const { qDebug() << message; }
};

SurgeRtspPlayer::SurgeRtspPlayer(QObject *parent) : videoSurface(nullptr), decoder(nullptr) {
    static InternalSurgeCoreLogger *internalSurgeCoreLogger;
    if (internalSurgeCoreLogger == nullptr) {
        internalSurgeCoreLogger = new InternalSurgeCoreLogger();
        SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
        logger.SetLoggingDelegate(internalSurgeCoreLogger);
    }

    surgeCoreDelegate = new InternalSurgeDelegateWrapper(this);
    this->rtspClient = new Surge::RtspClient(surgeCoreDelegate);
    diagnosticsTracker = new DiagnosticsTracker(rtspClient);
    diagnostics = diagnosticsTracker;
}

SurgeRtspPlayer::~SurgeRtspPlayer() {
    qInfo() << "Destroying Surge player";

    if (rtspClient != nullptr) {
        rtspClient->Disconnect();
        delete rtspClient;
    }

    if (decoder != nullptr) {
        delete decoder;
    }

    if (diagnosticsTracker != nullptr) {
        diagnosticsTracker->StopTracking();
        diagnosticsTracker->deleteLater();
    }

    delete surgeCoreDelegate;
}

void SurgeRtspPlayer::InitiatePlaybackOf(const QString &url) {
    InitiatePlaybackOf(url, "", "");
}

void SurgeRtspPlayer::InitiatePlaybackOf(const QString &url,
                                         const QString &username,
                                         const QString &password) {
    InitiatePlaybackOf(url, username, password, NULL, NULL);
}

void SurgeRtspPlayer::InitiatePlaybackOf(const QString &url,
                                         const QString &username,
                                         const QString &password,
                                         const QDateTime &startTime,
                                         const QDateTime &endTime) {
    InitiatePlaybackOf(url, username, password, &startTime, &endTime);
}

void SurgeRtspPlayer::InitiatePlaybackOf(const QString &url,
                                         const QString &username,
                                         const QString &password,
                                         const QDateTime *startTime,
                                         const QDateTime *endTime) {
    qInfo() << "Initating playback of " << url;

    this->url = url;
    this->username = username;
    this->password = password;
    this->urlProtocol = UrlProtocolFromString(url);

    SetTimeRange(startTime, endTime);

    if (urlProtocol != RTSP) {
        emit StreamInitiationFailed();
        qWarning() << "Url protocol is not RTSP.";
        return;
    }

    rtspClient->Connect(url.toStdString(), [this, username, password] (bool result) {
        if (!result) {
            emit StreamInitiationFailed();
            return;
        }

        rtspClient->SetCredentials(username.toStdString(), password.toStdString());

        rtspClient->Describe([this] (const Surge::DescribeResponse *describeResponse) {
            if (HandleAnyInitializationErrors(describeResponse)) {
                return;
            }

            auto sessionDescription = SelectPreferredSessionDescription(describeResponse->GetSessionDescriptions());

            qInfo() << "Detected video codec" << QString::fromStdString(sessionDescription.GetTypeAsString());
            InitializeDecoder(sessionDescription);

            rtspClient->Setup(sessionDescription, [this] (const Surge::SetupResponse *setupResponse) {
                if (HandleAnyInitializationErrors(setupResponse)) {
                    return;
                }

                rtspClient->Play([this] (Surge::RtspResponse *playResponse) {
                    if (HandleAnyInitializationErrors(playResponse)) {
                        return;
                    }

                    emit StreamInitiated();
                    emit StreamStarted();

                    diagnosticsTracker->StartTracking();

                    delete playResponse;
                });
                delete setupResponse;
            });
            delete describeResponse;
        });
    });
}

bool SurgeRtspPlayer::HandleAnyInitializationErrors(const Surge::RtspResponse *response) {
    auto successResponse = response != nullptr && response->Ok();
    if (!successResponse) {
        emit StreamInitiationFailed();
        delete response;
    }

    return !successResponse;
}

void SurgeRtspPlayer::Stop() {
    qInfo() << "Stopping playback of " << this->url;

    this->rtspClient->Teardown([this] (bool teardownResult) {
        if (!teardownResult) {
            qCritical() << "Stream TEARDOWN failed, attempting to force disconnect.";
        }

        ResetPlayer();

        emit StreamStopped();
    });
}

void SurgeRtspPlayer::ResetPlayer() {
    if (this->rtspClient != nullptr) {
        this->rtspClient->Disconnect();
    }

    if (decoder != nullptr) {
        delete this->decoder;
        this->decoder = nullptr;
    }

    if (diagnosticsTracker != nullptr) {
        diagnosticsTracker->StopTracking();
    }
}

void SurgeRtspPlayer::Play() {
    rtspClient->Play([this] (Surge::RtspResponse *playResponse) {
        if (playResponse->Ok()) {
            emit StreamStarted();
            diagnosticsTracker->StartTracking();
        } else {
            qCritical() << "Trying to play the stream has failed. Try restarting the stream.";
        }

        delete playResponse;
    });
}

void SurgeRtspPlayer::Pause() {
    rtspClient->Pause([this] (Surge::RtspResponse *pauseResponse) {
        if (pauseResponse->Ok()) {
            emit StreamPaused();
            diagnosticsTracker->StopTracking();
        } else {
            qCritical() << "Trying to pause the stream has failed. Try restarting the stream.";
        }

        delete pauseResponse;
    });
}

void SurgeRtspPlayer::Seek(QDateTime &startTime, QDateTime &endTime) {
    auto coreStartTime = DateTimeConverter::ToSurgeCoreDateTime(&startTime);
    auto coreEndTime = DateTimeConverter::ToSurgeCoreDateTime(&endTime);

    Pause();

    rtspClient->SetStartTime(coreStartTime);
    rtspClient->SetEndTime(coreEndTime);

    Play();
}

bool SurgeRtspPlayer::IsInterleavedTcpTransport() const {
    return rtspClient->IsInterleavedTransport();
}

void SurgeRtspPlayer::SetInterleavedTcpTransport(bool interleavedTcpTransport) {
    rtspClient->SetInterleavedTransport(interleavedTcpTransport);
}

bool SurgeRtspPlayer::IsTlsCertificateValidationEnabled() const {
    return rtspClient->IsTLSCertificateValidationEnabled();
}

void SurgeRtspPlayer::SetTlsCertificateValidationEnabled(bool tlsCertificateValidationEnabled) {
    rtspClient->SetTLSCertificateValidationEnabled(tlsCertificateValidationEnabled);
}

bool SurgeRtspPlayer::IsTlsSelfSignedCertificateAllowed() const {
    return rtspClient->TLSSelfSignedCertsAllowed();
}

void SurgeRtspPlayer::SetTlsSelfSignedCertificateAllowed(bool tlsSelfSignedCertificateAllowed) {
    rtspClient->SetTLSSelfSignedCertsAllowed(tlsSelfSignedCertificateAllowed);
}

void SurgeRtspPlayer::SetTlsTrustedCertificatePath(QString filePath) {
    rtspClient->SetTLSTrustedCertificate(filePath.toStdString());
}

void SurgeRtspPlayer::SetTimeRange(const QDateTime *startTime, const QDateTime *endTime) {
    auto coreStartTime = DateTimeConverter::ToSurgeCoreDateTime((QDateTime *)startTime);
    auto coreEndTime = DateTimeConverter::ToSurgeCoreDateTime((QDateTime *)endTime);

    this->rtspClient->SetTimeRange((SurgeUtil::DateTime*)startTime, (SurgeUtil::DateTime*)endTime);
}

void SurgeRtspPlayer::HandleStreamTimeout() {
    ResetPlayer();
    emit StreamTimeout();
}

Surge::SessionDescription SurgeRtspPlayer::SelectPreferredSessionDescription(const std::vector<Surge::SessionDescription> &sessionDescriptions) {
    return sessionDescriptions[0];
}

void SurgeRtspPlayer::InitializeDecoder(Surge::SessionDescription &sessionDescription) {
    if (decoder != nullptr) {
        delete decoder;
    }

    decoder = decoderFactory.CreateDecoder(sessionDescription.GetType(), videoSurface, &dispatcher, diagnosticsTracker);
    connect(decoder, &Decoder::PlaybackStarted, this, &SurgeRtspPlayer::PlaybackStarted);

}

void SurgeRtspPlayer::SetVideoSurface(QAbstractVideoSurface *videoSurface) {
    this->videoSurface = videoSurface;
    if (decoder != nullptr) {
        this->decoder->ChangeVideoSurface(videoSurface);
    }
}

void SurgeRtspPlayer::ClientReceivedFrame(const unsigned char * frameBuffer,
                         size_t length,
                         int32_t width,
                         int32_t height,
                         int32_t presentationTime,
                         int32_t duration) {
    if (decoder != nullptr) {
        auto qByteArray = QByteArray::fromRawData((const char *)frameBuffer, length);
        decoder->DecodeFrameBuffer(qByteArray, width, height, presentationTime, duration);
    }
}
