/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.player;

import android.content.Context;
import android.content.res.Resources;
import android.view.TextureView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.Date;

import co.instil.surge.authentication.SurgeAuthenticator;
import co.instil.surge.callbacks.PlayerCallback;
import co.instil.surge.client.DescribeResponse;
import co.instil.surge.client.ExtendedHeader;
import co.instil.surge.client.Response;
import co.instil.surge.client.RtspClient;
import co.instil.surge.client.RtspClientDelegate;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.client.SurgeVideoScale;
import co.instil.surge.client.SurgeVideoView;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.decoders.DecoderFactory;
import co.instil.surge.diagnostics.DiagnosticsTracker;
import co.instil.surge.diagnostics.SurgeDiagnostics;
import co.instil.surge.diagnostics.SurgeDiagnosticsDelegate;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

import static co.instil.surge.client.SessionType.H264;
import static co.instil.surge.client.SessionType.H265;
import static co.instil.surge.client.SessionType.MJPEG;
import static co.instil.surge.client.SessionType.MP4V;

/**
 *
 */
public class SurgeRtspPlayer implements AutoCloseable, RtspClientDelegate {
    private static final Logger LOGGER = LoggerFactory.getLogger(SurgeRtspPlayer.class);

    protected RtspClient rtspClient;

    protected String url;
    protected String username;
    protected String password;
    protected SessionDescription[] sessionDescriptions;
    protected SessionDescription sessionDescription;

    protected ExtendedHeader extendedHeader;

    public SurgeRtspPlayerDelegate delegate;

    protected SurgeVideoView surface = null;
    private SurgeVideoScale videoScale = SurgeVideoScale.MATCH_ASPECT_RATIO;
    private Decoder decoder;

    private SurgeAuthenticator customAuthenticator;

    private DiagnosticsTracker diagnosticsTracker;

    public SurgeRtspPlayer() {
        rtspClient = generateRtspClient(false);
        diagnosticsTracker = new DiagnosticsTracker();
        diagnosticsTracker.setRtspClient(rtspClient);
    }

    protected RtspClient generateRtspClient(boolean interleavedTcpTransport) {
        return new RtspClient(this, interleavedTcpTransport);
    }

    /**
     * Initiate playback of the RTSP stream.
     * 
     * This will issue the required DESCRIBE, SETUP and PLAY RTSP requests to start stream playback.
     * Once completed, Surge will notify the user of the requests result via the supplied callback method.
     * If successful, the video stream will start playing on the provided surface associated with the SurgeRtspPlayer#Surface property.
     * 
     * \sa SurgeRtspPlayerDelegate
     * @param url The RTSP Stream URL
     * @param callback Callback method called once the stream has started playing, or has irrecoverably failed.
     */
    public synchronized void initiatePlaybackOf(String url, final PlayerCallback callback) {
        initiatePlaybackOf(url, "", "", null, null, callback);
    }

    /**
     * Initiate playback of a basic auth protected RTSP stream.
     *
     * This will issue the required DESCRIBE, SETUP and PLAY RTSP requests to start stream playback.
     * Once completed, Surge will notify the user of the requests result via the supplied callback method.
     * If successful, the video stream will start playing on the provided surface associated with the SurgeRtspPlayer#Surface property.
     *
     * \sa SurgeRtspPlayerDelegate
     * @param url The RTSP Stream URL
     * @param username Username used to authenticate the stream.
     * @param password Password associated with the provided username.
     * @param callback Callback method called once the stream has started playing, or has irrecoverably failed.
     */
    public synchronized void initiatePlaybackOf(String url, String username, String password, final PlayerCallback callback) {
        initiatePlaybackOf(url, username, password, null, null, callback);
    }

    /**
     * Initiate playback of a basic auth protected RTSP stream at a specified timestamp.
     *
     * This will issue the required DESCRIBE, SETUP and PLAY RTSP requests to start stream playback.
     * Once completed, Surge will notify the user of the requests result via the supplied callback method.
     * If successful, the video stream will start playing on the provided surface associated with the SurgeRtspPlayer#Surface property.
     *
     * \sa SurgeRtspPlayerDelegate
     * @param url The RTSP Stream URL
     * @param username Username used to authenticate the stream.
     * @param password Password associated with the provided username.
     * @param startTime If the RTSP stream supports recorded video, timestamp to start playing video from.
     * @param endTime If the RTSP stream supports recorded video, timestamp to finish playing video at.
     * @param callback Callback method called once the stream has started playing, or has irrecoverably failed.
     */
    public synchronized void initiatePlaybackOf(final String url, final String username, final String password, Date startTime, Date endTime, final PlayerCallback callback) {
        this.url = url;
        this.username = username;
        this.password = password;

        LOGGER.debug("Initating playback of {}", url);

        rtspClient.setTimeRange(startTime, endTime);

        rtspClient.connect(url, result -> {
            setCredentials(username, password);

            rtspClient.describe(rawResponse -> {
                DescribeResponse response = (DescribeResponse) rawResponse;

                if (errorResponseHasBeenHandled(response, callback)) {
                    return;
                }

                if (response.getSessionDescriptions() == null
                        || response.getSessionDescriptions().length == 0) {
                    callback.response(response.getStatusCode());
                    return;
                }

                for (SessionDescription sessionDescription : response.getSessionDescriptions()) {
                    LOGGER.debug(sessionDescription.toString());
                }

                setSessionDescriptions(response.getSessionDescriptions());
                if (sessionDescriptions.length > 0) {
                    setupStream(selectPreferredSessionDescription(getSessionDescriptions()), callback);
                } else {
                    throw new UnsupportedOperationException("No session description available, is the stream active?");
                }


                diagnosticsTracker.setDepreciatedDelegate(delegate);
                diagnosticsTracker.startTracking();
            });
        });
    }

    private void setCredentials(String username, String password) {
        if (username == null) {
            username = "";
        }
        if (password == null) {
            password = "";
        }
        rtspClient.setCredentials(username, password);
    }

    private void setupStream(SessionDescription sessionDescription, final PlayerCallback callback) {
        this.sessionDescription = sessionDescription;
        initialiseDecoder(sessionDescription);

        rtspClient.setup(sessionDescription, response -> {
            if (errorResponseHasBeenHandled(response, callback)) {
                return;
            }

            playStream(callback);
        });
    }

    private void playStream(final PlayerCallback callback) {
        rtspClient.play(response -> {
            if (errorResponseHasBeenHandled(response, callback)) {
                return;
            }

            callback.response(response.getStatusCode());
        });
    }

    private boolean errorResponseHasBeenHandled(Response response, PlayerCallback callback) {
        if (response == null) {
            callback.response(RtspErrorCode.UNKNOWN_FAILURE);
            return true;
        } else if (response.getStatusCode() !=  RtspErrorCode.SUCCESS) {
            callback.response(response.getStatusCode());
            return true;
        }

        return false;
    }

    protected void initialiseDecoder(SessionDescription sessionDescription) {
        LOGGER.info("Initializing new decoders");

        if (surface != null) {
            Decoder currentDecoder = decoder;
            decoder = null;

            if (sessionDescription.getType() == H264) {
                decoder = DecoderFactory.generateH264Decoder(surface, diagnosticsTracker);
            } else if (sessionDescription.getType() == MP4V) {
                decoder = DecoderFactory.generateMP4VDecoder(surface, diagnosticsTracker);
            } else if (sessionDescription.getType() == MJPEG) {
                decoder = DecoderFactory.generateMJPEGDecoder(surface, diagnosticsTracker);
            } else if (sessionDescription.getType() == H265) {
                decoder = DecoderFactory.generateH265Decoder(surface, diagnosticsTracker);
            }

            if (currentDecoder != null) {
                try {
                    currentDecoder.close();
                } catch (InterruptedException e) {
                    LOGGER.printStackTrace(e);
                }
            }
        }
    }

    /**
     * Resumes playback of a paused stream.
     */
    public void play() {
        LOGGER.debug("Starting/resuming playback of {}", url);
        rtspClient.play();
    }

    /**
     * Pause playback of the stream.
     */
    public void pause() {
        LOGGER.debug("Pausing playback of {}", url);
        rtspClient.pause();
    }

    /**
     *Stop playback of the stream, issuing the TEARDOWN RTSP request.
     * 
     * Once Stop has been requested, the stream cannot be restarted via the Play() command.
     * To restart a stream, please re-execute the InitiatePlaybackOf() command.
     */
    public void stop() {
        LOGGER.debug("Stopping playback of {}", url);
        diagnosticsTracker.stopTracking();
        if (decoder != null) {
            try {
                decoder.close();
                decoder = null;
            } catch (InterruptedException e) {
                LOGGER.printStackTrace(e);
            }
        }

        rtspClient.tearDown(response ->
                rtspClient.disconnect());
    }

    /**
     * Seek playback to a specified point in time.
     *
     * Seeking has to be supported by the RTSP stream for this transaction to succeed. If unsupported,
     * calling this method will likely cause the stream to stop all playback and timeout.
     * @param startTime Timestamp to start playing video from.
     * @param endTime Timestamp to finish playing video at.
     * @param callback Callback method called once the stream has started playing again, or has irrecoverably failed.
     */
    public void seek(Date startTime, Date endTime, PlayerCallback callback) {
        rtspClient.pause(response -> {
            if (errorResponseHasBeenHandled(response, callback)) {
                return;
            }

            rtspClient.setTimeRange(startTime, endTime);

            playStream(callback);
        });
    }

    /**
     * Seek playback to a specified point in time.
     *
     * Seeking has to be supported by the RTSP stream for this transaction to succeed. If unsupported,
     * calling this method will likely cause the stream to stop all playback and timeout.
     * @param startTime Timestamp to start playing video from.
     * @param endTime Timestamp to finish playing video at.
     */
    public void seek(Date startTime, Date endTime) {
        seek(startTime, endTime, statusCode -> {});
    }

    protected SessionDescription selectPreferredSessionDescription(SessionDescription[] sessionDescriptions) {
        return sessionDescriptions[0];
    }

    /**
     * The view that Surge will use to play the RTSP video stream on.
     * @param textureView TextureView to play the RTSP video on.
     */
    public void setPlayerView(TextureView textureView) {
        this.surface = new SurgeVideoView(textureView, videoScale);

        if (sessionDescription != null) {
            initialiseDecoder(sessionDescription);
        } else {
            LOGGER.error("No decoder generated as there are no available SessionDescriptions");
        }
    }

    /**
     * The view that Surge will use to play the RTSP video stream on.
     *
     * @return The view that Surge will use to play the RTSP video stream on.
     */
    public TextureView getPlayerView() {
        return surface.getTextureView();
    }

    @Override
    public void close() throws Exception {
        try {
            rtspClient.close();
            diagnosticsTracker.stopTracking();
        } catch (Exception e) {
            LOGGER.error("Failed to close and clean up the native RTSP Client");
        }

        if (decoder != null) {
            decoder.close();
        }

        LOGGER.info("Surge player has been destroyed.");
    }

    @Override
    public void clientDidTimeout() {
        LOGGER.error("RTSP client timed out - calling delegate");

        if (delegate != null) {
            delegate.rtspPlayerDidTimeout();
        }
    }

    @Override
    public void clientReceivedFrame(ByteBuffer byteBuffer,
                                    int width,
                                    int height,
                                    int presentationTime,
                                    int duration) {

        if (decoder != null) {
            decoder.decodeFrameBuffer(sessionDescription,
                    byteBuffer,
                    surface.getViewWidth(),
                    surface.getViewHeight(),
                    presentationTime,
                    duration);
        }
    }

    @Override
    public void clientReceivedExtendedHeader(ByteBuffer buffer, int length) {
        LOGGER.debug("Received extended header");
    }

    protected SessionDescription getCurrentSessionDescription() {
        return sessionDescription;
    }

    protected SessionDescription[] getSessionDescriptions() {
        return sessionDescriptions;
    }

    protected void setSessionDescriptions(SessionDescription[] sessionDescriptions) {
        this.sessionDescriptions = sessionDescriptions;
    }

    /**
     *
     * @return
     */
    public ExtendedHeader getExtendedHeader() {
        return extendedHeader;
    }

    /**
     * Frames per second measured from the currently playing stream.
     * @return Frames per second measured from the currently playing stream.
     * @deprecated use {@diagnostics #getFramesPerSecond()} instead.
     */
    @Deprecated
    public int getFramesPerSecond() {
        return diagnosticsTracker.getFramesPerSecond();
    }

    /**
     * Returns true if Surge is streaming video data by a TCP transport, rather than a UDP transport.
     * @return true if Surge is streaming video data by a TCP transport, rather than a UDP transport.
     */
    public boolean isInterleavedTransport() {
        return rtspClient.isInterleavedTransport();
    }

    /**
     * If true, Surge will stream video data via an interleaved TCP transport rather than via UDP
     * @param interleavedTcpTransport True if requiring a TCP transport, false if using a UDP transport.
     */
    public void setInterleavedTransport(boolean interleavedTcpTransport) {
        if (rtspClient.isInterleavedTransport() == interleavedTcpTransport) {
            return;
        }

        rtspClient = generateRtspClient(interleavedTcpTransport);
        diagnosticsTracker.setRtspClient(rtspClient);
    }

    /**
     * Time delay, in milliseconds, between Surge receiving a packet containing video data and the 
     * frame data being decoded for viewing. Used for error correction and packet reordering 
     * purposes for the UDP transport. 
     * 
     * Default: 200ms
     * @return Time delay, in milliseconds, between Surge receiving a packet containing video data and the
     * frame data being decoded for viewing
     */
    public int getPacketBufferDelay() {
        return rtspClient.getPacketBufferDelay();
    }

    /**
     * Set the time delay, in milliseconds, between Surge receiving a packet containing video data and the
     * frame data being decoded for viewing. Used for error correction and packet reordering 
     * purposes for the UDP transport.
     * @param packetBufferDelay Time to wait between urge receiving a packet containing video data and the
     * frame data being decoded for viewing.
     */
    public void setPacketBufferDelay(int packetBufferDelay) {
        rtspClient.setPacketBufferDelay(packetBufferDelay);
    }

    /**
     * Enables/disables TLS certificate validation when streaming from a TLS protected RTSP stream. Default: true.
     * @param tlsCertificateValidationEnabled Enable/disable disables TLS certificate validation.
     */
    public void setTLSCertificateValidationEnabled(boolean tlsCertificateValidationEnabled) {
        rtspClient.setTLSCertificateValidationEnabled(tlsCertificateValidationEnabled);
    }

    /**
     * Is TLS certificate validation enabled or disabled, when streaming from a TLS protected RTSP stream?
     * @return Is TLS certificate validation enabled or disabled, when streaming from a TLS protected RTSP stream?
     */
    public boolean isTLSCertificateValidationEnabled() {
        return rtspClient.isTLSCertificateValidationEnabled();
    }

    /**
     * Allows/disallows self signed TLS certificates from being used when streaming from a TLS protected RTSP stream. Default: true.
     * @param tlsSelfSignedCertsAllowed Allow/disallow self signed TLS certificates.
     */
    public void setTLSSelfSignedCertsAllowed(boolean tlsSelfSignedCertsAllowed) {
        rtspClient.setTLSSelfSignedCertsAllowed(tlsSelfSignedCertsAllowed);
    }

    /**
     * Are self signed certifiates allowed for streaming from a TLS protected RTSP stream?
     * @return True if self signed certificates are allowed for streaming from a TLS protected RTSP stream, otherwise false
     */
    public boolean tlsSelfSignedCertsAllowed() {
        return rtspClient.tlsSelfSignedCertsAllowed();
    }

    /**
     * Optional: The path to a trusted root certificate used to validate a TLS certificate received through the TLS handshaking process.
     * @param fileUrl URL of the root certificate.
     */
    public void setTLSTrustedCertificate(String fileUrl) {
        rtspClient.setTLSTrustedCertificate(fileUrl);
    }

    /**
     * Optional: The path to a trusted root certificate used to validate a TLS certificate received through the TLS handshaking process.
     * @param rawResourceId ID of the raw resource pointing to the root certificate to load into Surge.
     * @param context Context of the running app.
     */
    public void setTLSTrustedCertificate(int rawResourceId, Context context) {
        File cachefile = extractResourceIntoCacheFile(rawResourceId, context);
        setTLSTrustedCertificate(cachefile.getAbsolutePath());
    }

    /**
     * Gets the current custom authenticator assigned to Surge, being used to provide authentication to the stream.
     * @return The current custom authenticator assigned to Surge, being used to provide authentication to the stream.
     */
    public SurgeAuthenticator getAuthenticator() {
        return customAuthenticator;
    }

    /**
     * Optional: Sets a custom authenticator to be used to authenticate Surge with a stream.
     * @param authenticator Custom authenticator to be used to authenticate Surge with a stream.
     */
    public void setAuthenticator(SurgeAuthenticator authenticator) {
        if (customAuthenticator != null) {
            rtspClient.RemoveAuthenticator(1);
        }

        customAuthenticator = authenticator;

        rtspClient.AddAuthenticator(customAuthenticator);
    }

    /**
     * Sets the scaling mode on the video player view.
     *
     * Default: MATCH_ASPECT_RATIO
     *
     * @param videoScale Scaling mode to change the video player view to
     */
    public void setVideoScale(SurgeVideoScale videoScale) {
        this.videoScale = videoScale;
        if (surface != null) {
            surface.setVideoScale(videoScale);
        }
    }

    /**
     * Gets the current scaling mode on the video player view.
     *
     * @return The current scaling mode on the video player view.
     */
    public SurgeVideoScale getVideoScale() {
        return videoScale;
    }

    public void setDelegate(SurgeRtspPlayerDelegate delegate) {
        this.delegate = delegate;
        diagnosticsTracker.setDepreciatedDelegate(delegate);
    }

    public SurgeDiagnostics getDiagnostics() {
        return diagnosticsTracker;
    }

    public void setDiagnosticsDelegate(SurgeDiagnosticsDelegate delegate) {
        diagnosticsTracker.setDelegate(delegate);
    }


    private File extractResourceIntoCacheFile(int rawResourceId, Context context) {
        File file = null;

        try (InputStream rootCertificate = context.getResources().openRawResource(rawResourceId)) {
            File tmpDir = context.getCacheDir();
            file = File.createTempFile(String.valueOf(rawResourceId), ".tmp", tmpDir);
            file.deleteOnExit();

            try (OutputStream outStream = new FileOutputStream(file)) {
                byte[] buffer = new byte[8 * 1024];
                int bytesRead;
                while ((bytesRead = rootCertificate.read(buffer)) != -1) {
                    outStream.write(buffer, 0, bytesRead);
                }
            }

        } catch (Resources.NotFoundException e) {
            LOGGER.error("Could not find file in the app resources with ID " + rawResourceId);
            e.printStackTrace();
        } catch (IOException e) {
            LOGGER.error("Could not write to the cache directory.");
            e.printStackTrace();
        }

        return file;
    }
}
