#include "SurgePlayer.h"
#include <RtspClient.h>
#include <QDebug>

SurgePlayer::SurgePlayer(QObject *parent) :  surgeRtspPlayer(new SurgeRtspPlayer()), mediaPlayer(new QMediaPlayer()), diagnosticsTracker(nullptr) , diagnostics(new Diagnostics()) { }

SurgePlayer::~SurgePlayer() {
    diagnosticsTracker.StopTracking();

    surgeRtspPlayer->deleteLater();
    mediaPlayer->deleteLater();
    delete diagnostics;
}

void SurgePlayer::InitiatePlaybackOf(const QString &url) {
    InitiatePlaybackOf(url, "", "");
}

void SurgePlayer::InitiatePlaybackOf(const QString &url,
                                     const QString &username,
                                     const QString &password) {
    InitiatePlaybackOf(url, username, password, NULL, NULL);
}

void SurgePlayer::InitiatePlaybackOf(const QString &url,
                                     const QString &username,
                                     const QString &password,
                                     const QDateTime &startTime,
                                     const QDateTime &endTime) {
    InitiatePlaybackOf(url, username, password, &startTime, &endTime);
}

void SurgePlayer::InitiatePlaybackOf(const QString &url, const QString &username, const QString &password, const QDateTime *startTime, const QDateTime *endTime) {
    DisconnectAllSignals();
    Stop();

    this->url = url;
    this->username = username;
    this->password = password;
    this->urlProtocol = UrlProtocolFromString(url);

    AttachVideoSurface();
    if (urlProtocol == RTSP) {
        ConnectRtspSignals();

        surgeRtspPlayer->InitiatePlaybackOf(url, username, password);
    } else if (urlProtocol == HTTP) {
        qInfo() << "Initating playback of" << url;

        ConnectHttpSignals();

        mediaPlayer->setMedia(GenerateMjpegPlaybackUrl());
        mediaPlayer->play();

        diagnosticsTracker.UpdateVideoFormat(VideoFormat::MJPEG);
    } else {
        qWarning() << "Unrecognised protocol in url";
    }
}

QUrl SurgePlayer::CreateUrlWithUserInfo() {
    auto qUrl = QUrl(this->url);

    if (!this->username.isEmpty()) {
        qUrl.setUserName(this->username);
    }

    if (!this->password.isEmpty()) {
        qUrl.setPassword(this->password);
    }

    return qUrl;
}

QUrl SurgePlayer::GenerateMjpegPlaybackUrl() {
    auto url = CreateUrlWithUserInfo();
    QString playbackString;

#if defined(Q_PROCESSOR_ARM) && defined(Q_OS_LINUX)
    playbackString.append("gst-pipeline: souphttpsrc location=").append(url.toString()).append(" do-timestamp=true is_live=true ! multipartdemux ! vpudec ! qtvideosink sync=false");
#elif defined(Q_OS_LINUX)
    playbackString.append("gst-pipeline: souphttpsrc location=").append(url.toString()).append(" do-timestamp=true is_live=true ! multipartdemux ! jpegdec ! qtvideosink sync=false");
#endif

    return QUrl(playbackString);
}

void SurgePlayer::DisconnectAllSignals() {
    surgeRtspPlayer->disconnect();
    surgeRtspPlayer->diagnostics->disconnect();

    mediaPlayer->disconnect();
    diagnosticsTracker.disconnect();
}

void SurgePlayer::ConnectRtspSignals() {
    connect(surgeRtspPlayer, &SurgeRtspPlayer::StreamInitiated, this, &SurgePlayer::StreamInitiated);
    connect(surgeRtspPlayer, &SurgeRtspPlayer::StreamInitiationFailed, this, &SurgePlayer::StreamInitiationFailed);
    connect(surgeRtspPlayer, &SurgeRtspPlayer::PlaybackStarted, this, &SurgePlayer::PlaybackStarted);
    connect(surgeRtspPlayer, &SurgeRtspPlayer::StreamStarted, this, &SurgePlayer::StreamStarted);
    connect(surgeRtspPlayer, &SurgeRtspPlayer::StreamPaused, this, &SurgePlayer::StreamPaused);
    connect(surgeRtspPlayer, &SurgeRtspPlayer::StreamStopped, this, &SurgePlayer::StreamStopped);
    connect(surgeRtspPlayer, &SurgeRtspPlayer::StreamTimeout, this, &SurgePlayer::StreamTimeout);

    connect(surgeRtspPlayer->diagnostics, &Diagnostics::FramerateChanged, diagnostics, &Diagnostics::FramerateChanged);
    connect(surgeRtspPlayer->diagnostics, &Diagnostics::BitrateUpdated, diagnostics, &Diagnostics::BitrateUpdated);
    connect(surgeRtspPlayer->diagnostics, &Diagnostics::PacketCountUpdated, diagnostics, &Diagnostics::PacketCountUpdated);
    connect(surgeRtspPlayer->diagnostics, &Diagnostics::MediaSizeChanged, diagnostics, &Diagnostics::MediaSizeChanged);
    connect(surgeRtspPlayer->diagnostics, &Diagnostics::VideoFormatChanged, diagnostics, &Diagnostics::VideoFormatChanged);
}

void SurgePlayer::ConnectHttpSignals() {
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &SurgePlayer::StateChangedOnHttpStream, Qt::DirectConnection);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &SurgePlayer::MediaStatusOnHttpStream, Qt::DirectConnection);
    connect(mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &SurgePlayer::PlaybackErrorOnHttpStream, Qt::DirectConnection);

    connect(&diagnosticsTracker, &Diagnostics::FramerateChanged, diagnostics, &Diagnostics::FramerateChanged);
    connect(&diagnosticsTracker, &Diagnostics::BitrateUpdated, diagnostics, &Diagnostics::BitrateUpdated);
    connect(&diagnosticsTracker, &Diagnostics::MediaSizeChanged, diagnostics, &Diagnostics::MediaSizeChanged);
    connect(&diagnosticsTracker, &Diagnostics::VideoFormatChanged, diagnostics, &Diagnostics::VideoFormatChanged);
}

void SurgePlayer::StateChangedOnHttpStream(QMediaPlayer::State state) {
    switch (state) {
        case QMediaPlayer::StoppedState:
            emit StreamStopped();
            diagnosticsTracker.StopTracking();
            break;
        case QMediaPlayer::PlayingState:
            emit PlaybackStarted();
            diagnosticsTracker.StartTracking();
            break;
        case QMediaPlayer::PausedState:
            emit StreamPaused();
            diagnosticsTracker.StopTracking();
            break;
    }
}

void SurgePlayer::PlaybackErrorOnHttpStream(QMediaPlayer::Error error) {
    switch (error) {
    case QMediaPlayer::NetworkError:
        emit StreamTimeout();
        diagnosticsTracker.StopTracking();
        break;
    default:
        emit StreamInitiationFailed();
        break;
    }
}

void SurgePlayer::MediaStatusOnHttpStream(QMediaPlayer::MediaStatus status) {
    switch (status) {
    case QMediaPlayer::LoadingMedia:
        emit StreamStarted();
        break;
    }
}

void SurgePlayer::Play() {
    if (urlProtocol == RTSP) {
        surgeRtspPlayer->Play();
    } else if (urlProtocol == HTTP) {
        mediaPlayer->play();
    }
}

void SurgePlayer::Pause() {
    if (urlProtocol == RTSP) {
        surgeRtspPlayer->Pause();
    } else if (urlProtocol == HTTP) {
        mediaPlayer->pause();
    }
}

void SurgePlayer::Stop() {
    if (urlProtocol == RTSP) {
        surgeRtspPlayer->Stop();
    } else if (urlProtocol == HTTP) {
        mediaPlayer->stop();
        videoSurface->present(QVideoFrame());
    }
}

bool SurgePlayer::IsInterleavedTcpTransport() const {
    if (urlProtocol == RTSP) {
        return surgeRtspPlayer->IsInterleavedTcpTransport();
    } else {
        return true;
    }
}

void SurgePlayer::SetInterleavedTcpTransport(bool interleavedTcpTransport) {
    surgeRtspPlayer->SetInterleavedTcpTransport(interleavedTcpTransport);
}

void SurgePlayer::SetVideoSurface(QAbstractVideoSurface *videoSurface) {
    this->videoSurface = new SurgeVideoSurface(videoSurface);
    AttachVideoSurface();
}

void SurgePlayer::AttachVideoSurface() {
    if (url.isEmpty() || videoSurface == nullptr) {
        return;
    }

    if (urlProtocol == RTSP) {
        surgeRtspPlayer->SetVideoSurface(videoSurface);
    } else if (urlProtocol == HTTP) {
        mediaPlayer->setVideoOutput(videoSurface);
        connect(videoSurface, &SurgeVideoSurface::FrameSize, &diagnosticsTracker, &DiagnosticsTracker::UpdateMediaSize);
        connect(videoSurface, &SurgeVideoSurface::FrameBitrate, &diagnosticsTracker, &DiagnosticsTracker::TrackNewFrameOfSize);
    }
}
