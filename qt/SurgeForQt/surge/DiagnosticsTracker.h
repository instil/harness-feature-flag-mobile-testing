#ifndef DIAGNOSTICSTRACKER_H
#define DIAGNOSTICSTRACKER_H

#include <Diagnostics.h>
#include <QObject>
#include <QTimer>
#include <SurgeCoreDeclarations.h>
#include <QThread>

class DiagnosticsTracker : public Diagnostics {
    Q_OBJECT

public:
    DiagnosticsTracker(Surge::RtspClient *rtspClient);
    ~DiagnosticsTracker();

    void StartTracking();
    void StopTracking();

    void TrackNewFrameOfSize(int frameSize);
    void UpdateMediaSize(int width, int height);
    void UpdateVideoFormat(VideoFormat videoFormat);

private slots:
    void Update();

private:
    QTimer *timer;
    QThread* thread;
    Surge::RtspClient *rtspClient;

    int bitrateCounter;
    int fpsCounter;
    int currentWidth = -1;
    int currentHeight = -1;
};

#endif // DIAGNOSTICSTRACKER_H
