/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.diagnostics

import android.os.Handler
import co.instil.surge.diagnostics.SurgeDiagnostics
import android.os.Looper
import android.util.Size
import co.instil.surge.diagnostics.SurgeDiagnosticsDelegate
import co.instil.surge.player.SurgeRtspPlayerDelegate
import co.instil.surge.client.RtspClient
import co.instil.surge.diagnostics.RtpPacketBufferInfo

/**
 * This can track a number of different diagnostics from multiple data sources throughout Surge and
 * can emit them to the user via a diagnostics delegate.
 */
class DiagnosticsTracker : SurgeDiagnostics {
    override var framesPerSecond = 0
        private set
    override var bitrate = 0
        private set
    override var outOfOrderPacketsReceived: Long = 0
        private set
    override var successfulPacketsReceived: Long = 0
        private set
    override var lostPackets: Long = 0
        private set
    override var mediaDimensions = Size(0, 0)
        private set
    private var fpsCounter = 0
    private var bitrateCounter = 0

    private val handler = Handler(Looper.getMainLooper())
    private val runnable = Runnable { this.run() }
    private val timeBetweenUpdates = 1000
    private var threadIsRunning = false

    private var client: RtspClient? = null
    private var delegate: SurgeDiagnosticsDelegate? = null
    private var depreciatedDelegate: SurgeRtspPlayerDelegate? = null

    fun startTracking() {
        if (!threadIsRunning) {
            handler.postDelayed(runnable, timeBetweenUpdates.toLong())
            threadIsRunning = true
        }
    }

    fun stopTracking() {
        threadIsRunning = false
        handler.removeCallbacks(runnable)
    }

    fun trackNewFrameOfSize(bitrate: Int) {
        bitrateCounter += bitrate
        ++fpsCounter
    }

    fun trackNewFrameDimensions(mediaDimensions: Size) {
        if (this.mediaDimensions != mediaDimensions) {
            this.mediaDimensions = mediaDimensions
            delegate?.rtspPlayerDidChangeMediaSize(mediaDimensions)
        }
    }

    fun trackNewFrameDimensions(width: Int, height: Int) {
        trackNewFrameDimensions(Size(width, height))
    }

    private fun run() {
        calculateFramerate()
        calculateBitrate()
        trackRtpPacketBufferInfo()
        if (threadIsRunning) {
            handler.postDelayed(runnable, timeBetweenUpdates.toLong())
        }
    }

    private fun calculateFramerate() {
        framesPerSecond = fpsCounter
        fpsCounter = 0

        delegate?.rtspPlayerDidObservePlaybackFrameRate(framesPerSecond)
        if (depreciatedDelegate != delegate) {
            depreciatedDelegate?.rtspPlayerDidUpdateFps(framesPerSecond)
        }
    }

    private fun calculateBitrate() {
        bitrate = bitrateCounter
        bitrateCounter = 0
        delegate?.rtspPlayerDidObservePlaybackBitrate(bitrate)
    }

    private fun trackRtpPacketBufferInfo() {
        client?.diagnosticsOnPacketLoss?.let { packetBufferInfo ->
            successfulPacketsReceived = packetBufferInfo.successfulPacketsCount
            lostPackets = packetBufferInfo.missedPacketsCount
            outOfOrderPacketsReceived = packetBufferInfo.oooPacketsCount
            delegate?.rtspPlayerDidObservePacketLossDiagnostics(lostPackets, successfulPacketsReceived, outOfOrderPacketsReceived)
        }
    }

    fun setRtspClient(client: RtspClient) {
        this.client = client
    }

    fun setDelegate(delegate: SurgeDiagnosticsDelegate) {
        this.delegate = delegate
    }

    fun setDepreciatedDelegate(depreciatedDelegate: SurgeRtspPlayerDelegate) {
        this.depreciatedDelegate = depreciatedDelegate
    }
}