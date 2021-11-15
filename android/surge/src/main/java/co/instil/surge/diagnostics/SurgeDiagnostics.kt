/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.diagnostics

import android.util.Size
import co.instil.surge.client.SessionType

interface SurgeDiagnostics {
    val framesPerSecond: Int
    val bitrate: Int
    val outOfOrderPacketsReceived: Long
    val successfulPacketsReceived: Long
    val lostPackets: Long
    val mediaDimensions: Size
    val mediaFormat: SessionType
}