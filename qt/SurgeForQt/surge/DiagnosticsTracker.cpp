#include "DiagnosticsTracker.h"

#include "RtspClient.h"

#include "QDebug"

DiagnosticsTracker::DiagnosticsTracker(Surge::RtspClient *rtspClient) : rtspClient(rtspClient), bitrateCounter(0), fpsCounter(0) {
    thread = new QThread(this);
    timer = new QTimer(0);
    timer->setInterval(1000);
    timer->moveToThread(thread);

    qRegisterMetaType<VideoFormat>();
    connect(thread, SIGNAL(started()), timer, SLOT(start()), Qt::DirectConnection);
    connect(thread, SIGNAL(finished()), timer, SLOT(stop()), Qt::DirectConnection);
    connect(timer, &QTimer::timeout, this, &DiagnosticsTracker::Update);
}

DiagnosticsTracker::~DiagnosticsTracker() {
    if (thread->isRunning()) {
        StopTracking();
    }

    timer->deleteLater();
    thread->deleteLater();
}

void DiagnosticsTracker::StartTracking() {
    qDebug() << "Diagnostics Tracker Started";
    thread->start();
}

void DiagnosticsTracker::StopTracking() {
    qDebug() << "Diagnostics Tracker Stopped";
    thread->quit();
    thread->wait();
}

void DiagnosticsTracker::TrackNewFrameOfSize(int frameSize) {
    bitrateCounter += frameSize;
    ++fpsCounter;
}

void DiagnosticsTracker::UpdateMediaSize(int width, int height) {
    if (currentWidth != width || currentHeight != height) {
        currentWidth = width;
        currentHeight = height;
        emit MediaSizeChanged(width, height);
    }
}

void DiagnosticsTracker::UpdateVideoFormat(VideoFormat videoFormat) {
    emit VideoFormatChanged(videoFormat);
}

void DiagnosticsTracker::Update() {
    if (rtspClient != nullptr) {
        auto packetBufferInfo = rtspClient->GetDiagnosticsOnPacketLoss();

        emit PacketCountUpdated(packetBufferInfo.missedPacketsCount,
                                packetBufferInfo.successfulPacketsCount,
                                packetBufferInfo.oooPacketsCount);
    }

    emit BitrateUpdated(bitrateCounter);
    bitrateCounter = 0;

    emit FramerateChanged(fpsCounter);
    fpsCounter = 0;
}
