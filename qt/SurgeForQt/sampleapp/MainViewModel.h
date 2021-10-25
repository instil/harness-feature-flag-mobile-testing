#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QString>
#include <QDebug>
#include <ViewModel.h>
#include <SurgeRtspPlayer.h>

class MainViewModel : public ViewModel {
    Q_OBJECT
    Q_PROPERTY(QString url MEMBER url NOTIFY urlChanged);
    Q_PROPERTY(SurgeRtspPlayer * player MEMBER player NOTIFY playerChanged);

public:
    MainViewModel(QObject *parent = nullptr);

public slots:
    void onStartStreamButtonPressed();
    void onStopStreamButtonPressed();
    void onPlayButtonPressed();
    void onPauseButtonPressed();

    void streamStarted();
    void streamStartupFailed();
    void streamHasStopped();
    void playbackStarted();
    void streamHasPaused();
    void framerateUpdated(int fps);
    void mediaSizeUpdated(int width, int height);

signals:
    void urlChanged();
    void playerChanged();

public:
    QString url;
    QString username;
    QString password;
    SurgeRtspPlayer *player = new SurgeRtspPlayer();
};

#endif // MAINVIEWMODEL_H
