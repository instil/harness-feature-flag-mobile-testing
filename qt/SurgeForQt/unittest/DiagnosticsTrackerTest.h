#ifndef DIAGNOSTICSTRACKERTEST_H
#define DIAGNOSTICSTRACKERTEST_H

#include <QtTest>
#include <RtspClient.h>
#include "DiagnosticsTracker.h"
#include "TestSuite.h"

class DiagnosticsTrackerTest : public TestSuite {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void ShouldStartAndStopTrackingWithoutCrashing();
    void ShouldEmitMediaSizeChanged();
    void ShouldNotEmitMediaSizeChangedWhenMediaSizeIsSameAsPrevious();
    void ShouldEmitSignalsOnUpdate();
    void ShouldUpdateOncePerSecond();
    void ShouldEmitVideoFormatChanged();

private:
    DiagnosticsTracker *target;
    Surge::RtspClient *rtspClient;
};

static DiagnosticsTrackerTest TEST_DIAGNOSTICSTRACKER;

#endif // DIAGNOSTICSTRACKERTEST_H
