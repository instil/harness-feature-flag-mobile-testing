/*
 * Copyright (c) 2016 Instil Software.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
