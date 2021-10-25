#include "MainViewModel.h"

MainViewModel::MainViewModel(QObject *parent) :
    ViewModel("main.qml"),
    url("rtsp://192.168.50.76/videoStreamId=1"),
    username(""),
    password("") {

    this->player->SetInterleavedTcpTransport(true);

    connect(player, &SurgeRtspPlayer::StreamInitiated, this, &MainViewModel::streamStarted);
    connect(player, &SurgeRtspPlayer::StreamInitiationFailed, this, &MainViewModel::streamStartupFailed);
    connect(player, &SurgeRtspPlayer::StreamStopped, this, &MainViewModel::streamHasStopped);
    connect(player, &SurgeRtspPlayer::PlaybackStarted, this, &MainViewModel::playbackStarted);
    connect(player, &SurgeRtspPlayer::StreamPaused, this, &MainViewModel::streamHasPaused);
    connect(player->diagnostics, &Diagnostics::FramerateChanged, this, &MainViewModel::framerateUpdated);
    connect(player->diagnostics, &Diagnostics::MediaSizeChanged, this, &MainViewModel::mediaSizeUpdated);
}

void MainViewModel::onStartStreamButtonPressed() {
    this->player->InitiatePlaybackOf(url, username, password);
}

void MainViewModel::onStopStreamButtonPressed() {
    this->player->Stop();
}

void MainViewModel::onPlayButtonPressed() {
    this->player->Play();
}

void MainViewModel::onPauseButtonPressed() {
    this->player->Pause();
}

void MainViewModel::streamStarted() {
    qInfo() << "Stream has started";
}

void MainViewModel::streamStartupFailed() {
    qInfo() << "Stream has failed to start";
}

void MainViewModel::streamHasStopped() {
    qInfo() << "Stream has stopped playing.";
}

void MainViewModel::playbackStarted() {
    qInfo() << "Stream has started playing video.";
}

void MainViewModel::streamHasPaused() {
    qInfo() << "Stream has paused.";
}

void MainViewModel::framerateUpdated(int fps) {
    qInfo() << "FPS:" << fps;
}

void MainViewModel::mediaSizeUpdated(int width, int height) {
    qInfo() << "Media Size:" << width << "x" << height;
}
