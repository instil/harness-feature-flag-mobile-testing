/*
 * Copyright (c) 2019 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.diagnostics;

import android.os.Handler;
import android.util.Size;

import java.util.Objects;

import co.instil.surge.client.RtspClient;
import co.instil.surge.player.SurgeRtspPlayerDelegate;

/**
 * This can track a number of different diagnostics from multple data sources throughout Surge and
 * can emit them to the user via a diagnostics delegate.
 */
public class DiagnosticsTracker implements SurgeDiagnostics {
    private int framesPerSecond = 0;
    private int bitrate = 0;
    private long outOfOrderPacketsReceived = 0;
    private long successfulPacketsReceived = 0;
    private long lostPackets = 0;
    private Size mediaDimensions = new Size(0, 0);

    private int fpsCounter = 0;
    private int bitrateCounter = 0;

    private Handler handler = new Handler();
    private boolean threadIsRunning = false;
    private final int timeBetweenUpdates = 1000;

    private SurgeDiagnosticsDelegate delegate;
    private SurgeRtspPlayerDelegate depreciatedDelegate;

    private RtspClient client;

    public void startTracking() {
        if (!threadIsRunning) {
            handler.postDelayed(this::run, timeBetweenUpdates);
            threadIsRunning = true;
        }
    }

    public void stopTracking() {
        threadIsRunning = false;
    }

    public void trackNewFrameOfSize(int bitrate) {
        bitrateCounter += bitrate;
        ++fpsCounter;
    }

    public void trackNewFrameDimensions(Size mediaDimensions) {
        if (!this.mediaDimensions.equals(mediaDimensions)) {
            this.mediaDimensions = mediaDimensions;

            if (delegate != null) {
                delegate.rtspPlayerDidChangeMediaSize(mediaDimensions);
            }
        }
    }

    public void trackNewFrameDimensions(int width, int height) {
        trackNewFrameDimensions(new Size(width, height));
    }

    private void run() {
        calculateFramerate();
        calculateBitrate();
        trackRtpPacketBufferInfo();

        if (threadIsRunning) {
            handler.postDelayed(this::run, timeBetweenUpdates);
        }
    }

    private void calculateFramerate() {
        framesPerSecond = fpsCounter;
        fpsCounter = 0;

        if (delegate != null) {
            delegate.rtspPlayerDidObservePlaybackFrameRate(framesPerSecond);
        }

        if (depreciatedDelegate != null && Objects.equals(depreciatedDelegate, delegate)) {
            depreciatedDelegate.rtspPlayerDidUpdateFps(framesPerSecond);
        }
    }

    private void calculateBitrate() {
        bitrate = bitrateCounter;
        bitrateCounter = 0;

        if (delegate != null) {
            delegate.rtspPlayerDidObservePlaybackBitrate(bitrate);
        }
    }

    private void trackRtpPacketBufferInfo() {
        RtpPacketBufferInfo packetBufferInfo = client.GetDiagnosticsOnPacketLoss();
        successfulPacketsReceived = packetBufferInfo.getSuccessfulPacketsCount();
        lostPackets = packetBufferInfo.getMissedPacketsCount();
        outOfOrderPacketsReceived = packetBufferInfo.getOooPacketsCount();

        if (delegate != null) {
            delegate.rtspPlayerDidObservePacketLossDiagnostics(lostPackets, successfulPacketsReceived, outOfOrderPacketsReceived);
        }
    }

    public void setRtspClient(RtspClient client) {
        this.client = client;
    }

    public void setDelegate(SurgeDiagnosticsDelegate delegate) {
        this.delegate = delegate;
    }

    public void setDepreciatedDelegate(SurgeRtspPlayerDelegate depreciatedDelegate) {
        this.depreciatedDelegate = depreciatedDelegate;
    }

    public int getFramesPerSecond() {
        return framesPerSecond;
    }

    public int getBitrate() {
        return bitrate;
    }

    public long getOutOfOrderPacketsReceived() {
        return outOfOrderPacketsReceived;
    }

    public long getSuccessfulPacketsReceived() {
        return successfulPacketsReceived;
    }

    public long getLostPackets() {
        return lostPackets;
    }

    public Size getMediaDimensions() {
        return mediaDimensions;
    }
}
