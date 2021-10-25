#include "DiagnosticsTrackerTest.h"

void DiagnosticsTrackerTest::init() {
    rtspClient = new Surge::RtspClient(nullptr);
    target = new DiagnosticsTracker(rtspClient);
}

void DiagnosticsTrackerTest::cleanup() {
    target->StopTracking();
    target->deleteLater();
}

void DiagnosticsTrackerTest::ShouldStartAndStopTrackingWithoutCrashing() {
    target->StartTracking();
    target->StopTracking();
    target->StartTracking();
    target->StopTracking();
}

void DiagnosticsTrackerTest::ShouldEmitMediaSizeChanged() {
    QSignalSpy mediaSizeChangedSpy(target, &Diagnostics::MediaSizeChanged);

    target->UpdateMediaSize(100, 100);

    QCOMPARE(mediaSizeChangedSpy.count(), 1);
}

void DiagnosticsTrackerTest::ShouldNotEmitMediaSizeChangedWhenMediaSizeIsSameAsPrevious() {
    QSignalSpy mediaSizeChangedSpy(target, &Diagnostics::MediaSizeChanged);

    target->UpdateMediaSize(100, 100);
    target->UpdateMediaSize(100, 100);

    QCOMPARE(mediaSizeChangedSpy.count(), 1);
}

void DiagnosticsTrackerTest::ShouldEmitSignalsOnUpdate() {
    QSignalSpy framerateChangedSpy(target, &Diagnostics::FramerateChanged);
    QSignalSpy bitrateUpdatedSpy(target, &Diagnostics::BitrateUpdated);
    QSignalSpy packetCountUpdatedSpy(target, &Diagnostics::PacketCountUpdated);

    target->StartTracking();

    QTest::qWait(1500);

    QCOMPARE(framerateChangedSpy.count(), 1);
    QCOMPARE(bitrateUpdatedSpy.count(), 1);
    QCOMPARE(packetCountUpdatedSpy.count(), 1);
}

void DiagnosticsTrackerTest::ShouldUpdateOncePerSecond() {
    QSignalSpy framerateChangedSpy(target, &Diagnostics::FramerateChanged);
    QSignalSpy bitrateUpdatedSpy(target, &Diagnostics::BitrateUpdated);
    QSignalSpy packetCountUpdatedSpy(target, &Diagnostics::PacketCountUpdated);

    target->StartTracking();

    QTest::qWait(2000);

    QCOMPARE(framerateChangedSpy.count(), 2);
    QCOMPARE(bitrateUpdatedSpy.count(), 2);
    QCOMPARE(packetCountUpdatedSpy.count(), 2);
}

void DiagnosticsTrackerTest::ShouldEmitVideoFormatChanged() {
    QSignalSpy videoFormatChangedSpy(target, &Diagnostics::VideoFormatChanged);

    target->UpdateVideoFormat(VideoFormat::H264);

    QCOMPARE(videoFormatChangedSpy.count(), 1);
    QCOMPARE(videoFormatChangedSpy.takeFirst().at(0).toInt(), VideoFormat::H264);
}
