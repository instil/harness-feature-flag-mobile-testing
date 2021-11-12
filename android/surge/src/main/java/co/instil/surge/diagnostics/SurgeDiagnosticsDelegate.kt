/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.diagnostics

import android.util.Size

interface SurgeDiagnosticsDelegate {
    fun rtspPlayerDidObservePlaybackFrameRate(frameRate: Int)
    fun rtspPlayerDidObservePlaybackBitrate(bitrate: Int)
    fun rtspPlayerDidObservePacketLossDiagnostics(packetLossCount: Long, successfulPacketCount: Long, oooPacketsCount: Long)
    fun rtspPlayerDidChangeMediaSize(mediaSize: Size)
}