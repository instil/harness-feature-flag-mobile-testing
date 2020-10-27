/*
 * Copyright (c) 2019 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.diagnostics;

public class RtpPacketBufferInfo {
    private long successfulPacketsCount;
    private long missedPacketsCount;
    private long oooPacketsCount;

    public RtpPacketBufferInfo(long successfulPacketsCount, long missedPacketsCount, long oooPacketsCount) {
        this.successfulPacketsCount = successfulPacketsCount;
        this.missedPacketsCount = missedPacketsCount;
        this.oooPacketsCount = oooPacketsCount;
    }

    public long getSuccessfulPacketsCount() {
        return successfulPacketsCount;
    }

    public long getMissedPacketsCount() {
        return missedPacketsCount;
    }

    public long getOooPacketsCount() {
        return oooPacketsCount;
    }
}
