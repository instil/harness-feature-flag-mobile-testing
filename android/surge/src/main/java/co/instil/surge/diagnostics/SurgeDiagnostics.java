/*
 * Copyright (c) 2019 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.diagnostics;

import android.util.Size;

public interface SurgeDiagnostics {
    int getFramesPerSecond();
    int getBitrate();
    long getOutOfOrderPacketsReceived();
    long getSuccessfulPacketsReceived();
    long getLostPackets();
    Size getMediaDimensions();
}
