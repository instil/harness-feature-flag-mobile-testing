/*
 * Copyright (c) 2019 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.diagnostics;

import android.util.Size;

public interface SurgeDiagnosticsDelegate {
    void rtspPlayerDidObservePlaybackFrameRate(int frameRate);
    void rtspPlayerDidObservePlaybackBitrate(int bitrate);
    void rtspPlayerDidObservePacketLossDiagnostics(long packetLossCount, long successfulPacketCount, long oooPacketsCount);
    void rtspPlayerDidChangeMediaSize(Size mediaSize);
}
