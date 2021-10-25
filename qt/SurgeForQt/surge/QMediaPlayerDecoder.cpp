#include <QMediaPlayerDecoder.h>

QMediaPlayerDecoder::QMediaPlayerDecoder(QAbstractVideoSurface *videoSurface, Dispatcher *dispatcher, DiagnosticsTracker *diagnosticsTracker) : surgeVideoSurface(nullptr), diagnosticsTracker(diagnosticsTracker), dispatcher(dispatcher), decoderIsRunning(false) {
    SetVideoSurface(videoSurface);

    dispatcher->DispatchToMainThread([this] {
            player = new QMediaPlayer(nullptr, QMediaPlayer::Flag::VideoSurface);
            player->setVideoOutput(surgeVideoSurface);
        });
    stream = new ByteStream();
    stream->open(QIODevice::ReadWrite);
}

QMediaPlayerDecoder::~QMediaPlayerDecoder() {
    stream->readAll();
    stream->close();

    DeleteDecoder();

    stream->deleteLater();

    if (surgeVideoSurface != nullptr) {
        surgeVideoSurface->present(QVideoFrame());
        surgeVideoSurface->deleteLater();
    }
}

void QMediaPlayerDecoder::DecodeFrameBuffer(QByteArray &frameBuffer, const int width, const int height, const int presentationTime, const int duration) {
    if (surgeVideoSurface == nullptr) {
        return;
    }

    if (diagnosticsTracker != nullptr) {
        diagnosticsTracker->TrackNewFrameOfSize(frameBuffer.size() * 8);
    }

    stream->write(frameBuffer);

    if (DecoderNeedsToBeStarted()) {
        StartDecoder();
        emit PlaybackStarted();

        if (diagnosticsTracker != nullptr) {
            diagnosticsTracker->UpdateVideoFormat(GetDecoderFormat());
        }
    }
}

void QMediaPlayerDecoder::StartDecoder() {
    dispatcher->DispatchToMainThread([this] {
        player->setMedia(GstreamerPipeline(), stream);
        player->play();
    });
}

bool QMediaPlayerDecoder::DecoderNeedsToBeStarted()  {
    if (!decoderIsRunning) {
        decoderIsRunning = true;
        return true;
    }

    return false;
}

void QMediaPlayerDecoder::ChangeVideoSurface(QAbstractVideoSurface *videoSurface) {
    qInfo() << "New video surface has been set" << videoSurface;
    SetVideoSurface(videoSurface);

    dispatcher->DispatchToMainThread([this] {
        player->setVideoOutput(surgeVideoSurface);
    });
}

void QMediaPlayerDecoder::SetVideoSurface(QAbstractVideoSurface *videoSurface) {
    if (videoSurface == nullptr) {
        qWarning() << "Decoder has been created without a valid video surface. Video may not play correctly until a valid video surface is provided.";
        return;
    }

    if (surgeVideoSurface != nullptr) {
        delete surgeVideoSurface;
    }

    surgeVideoSurface = new SurgeVideoSurface(videoSurface);

    if (diagnosticsTracker != nullptr) {
        connect(surgeVideoSurface, &SurgeVideoSurface::FrameSize, diagnosticsTracker, &DiagnosticsTracker::UpdateMediaSize);
    }
}

void QMediaPlayerDecoder::DeleteDecoder() {
    player->stop();

    auto localPlayer = player;
    connect(player, &QMediaPlayer::stateChanged, player, [localPlayer](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState) {
            qInfo() << "Deleting decoder";
            localPlayer->deleteLater();
        }
    });
}
