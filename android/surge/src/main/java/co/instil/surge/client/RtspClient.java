/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.client;

import co.instil.surge.authentication.SurgeAuthenticator;
import co.instil.surge.callbacks.BooleanCallback;
import co.instil.surge.callbacks.ResponseCallback;
import co.instil.surge.diagnostics.RtpPacketBufferInfo;

import java.util.Date;

/**
 *
 */
@SuppressWarnings("PMD")
public class RtspClient implements AutoCloseable {

    static {
        System.loadLibrary("surge");
        load();
    }

    private final long nativeClient;

    public RtspClient(RtspClientDelegate delegate) {
        this(delegate, false);
    }

    public RtspClient(RtspClientDelegate delegate, boolean isInterleavedTcpTransport) {
        nativeClient = generateNativeClientInstance(delegate, isInterleavedTcpTransport);
    }

    protected long generateNativeClientInstance(RtspClientDelegate delegate, boolean isInterleavedTcpTransport) {
        return createNativeClientInstance(delegate, isInterleavedTcpTransport);
    }

    private static native void load();

    private native long createNativeClientInstance(RtspClientDelegate delegate, boolean isInterleavedTcpTransport);

    public native void connect(String url, BooleanCallback callback);

    public native void disconnect();

    public native void setCredentials(String username, String password);

    public native void describe(ResponseCallback callback);

    public native void setup(SessionDescription sessionDescription, ResponseCallback callback);

    public native void play();

    public native void play(ResponseCallback callback);

    public native void pause();

    public native void pause(ResponseCallback callback);

    public native void options(ResponseCallback callback);

    public native void tearDown(ResponseCallback callback);

    public native void keepAlive(ResponseCallback callback);

    public native void setTimeRange(Date startTime, Date endtime);
    public native void setStartTime(Date startTime);
    public native void setEndTime(Date endtime);

    public native boolean isInterleavedTransport();

    public native int getPacketBufferDelay();

    public native void setPacketBufferDelay(int packetBufferDelay);

    public native void setTLSCertificateValidationEnabled(boolean tlsCertificateValidationEnabled);

    public native boolean isTLSCertificateValidationEnabled();

    public native void setTLSSelfSignedCertsAllowed(boolean tlsSelfSignedCertsAllowed);

    public native boolean tlsSelfSignedCertsAllowed();

    public native void setTLSTrustedCertificate(String fileUrl);

    public native void addAuthenticator(SurgeAuthenticator authenticator);

    public native void removeAuthenticator(SurgeAuthenticator authenticator);

    public native void removeAuthenticator(int index);

    public native RtpPacketBufferInfo getDiagnosticsOnPacketLoss();

    public native void close();
}
