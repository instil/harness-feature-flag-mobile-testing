/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.client;

import java.util.Date;

import co.instil.surge.callbacks.ResponseCallback;

/**
 *
 */
public class RtspClient implements AutoCloseable {

    static {
        System.loadLibrary("surge");
        load();
    }

    private final long nativeClient;

    public RtspClient(RtspClientDelegate delegate) {
        nativeClient = generateNativeClientInstance(delegate);
    }

    protected long generateNativeClientInstance(RtspClientDelegate delegate) {
        return createNativeClientInstance(delegate);
    }

    private static native void load();

    private native long createNativeClientInstance(RtspClientDelegate delegate);

    public native void options(ResponseCallback callback);

    public native void options(String url, ResponseCallback callback);

    public native void describe(String url, ResponseCallback callback);

    public native void describe(String url, String username, String password, ResponseCallback callback);

    public native void setup(SessionDescription sessionDescription, ResponseCallback callback);

    public native void play();

    public native void play(ResponseCallback callback);

    public native void pause();

    public native void pause(ResponseCallback callback);

    public native void setTimeRange(Date startTime, Date endtime);

    public native void setStartTime(Date startTime);

    public native void setEndTime(Date endTime);

    public native void tearDown();

    public native void keepAlive(ResponseCallback callback);

    public native void close();

    public native void stopStream();
}
