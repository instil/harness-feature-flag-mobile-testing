/*
 * Copyright (c) 2021 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */
package co.instil.surge.diagnostics

data class RtpPacketBufferInfo(val successfulPacketsCount: Long, val missedPacketsCount: Long, val oooPacketsCount: Long)