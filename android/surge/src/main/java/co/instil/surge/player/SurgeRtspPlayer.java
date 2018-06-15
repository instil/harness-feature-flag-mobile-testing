/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.player;

import java.nio.ByteBuffer;
import java.util.Date;

import co.instil.surge.callbacks.PlayerCallback;
import co.instil.surge.client.DescribeResponse;
import co.instil.surge.client.ExtendedHeader;
import co.instil.surge.client.RtspClient;
import co.instil.surge.client.RtspClientDelegate;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.client.SurgeSurface;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.decoders.DecoderFactory;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

import static co.instil.surge.client.SessionType.H264;
import static co.instil.surge.client.SessionType.MJPEG;
import static co.instil.surge.client.SessionType.MP4V;

/**
 *
 */
public class SurgeRtspPlayer implements AutoCloseable, RtspClientDelegate {

    private static Logger logger = LoggerFactory.getLogger(SurgeRtspPlayer.class);

    protected RtspClient rtspClient;

    protected String url;
    protected String username;
    protected String password;
    protected SessionDescription[] sessionDescriptions;
    protected SessionDescription sessionDescription;

    protected ExtendedHeader extendedHeader;

    public SurgeRtspPlayerDelegate delegate;

    protected SurgeSurface surface = null;
    private Decoder decoder;

    public SurgeRtspPlayer() {
        rtspClient = generateRtspClient(false);
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
     * @param surface The Surface that Surge will use to play the RTSP video stream on.
     * @param callback Callback method called once the stream has started playing, or has irrecoverably failed.
     */
    public synchronized void initiatePlaybackOf(String url, SurgeSurface surface, final PlayerCallback callback) {
        initiatePlaybackOf(url, surface, "", "", null, null, callback);
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
     * @param surface The Surface that Surge will use to play the RTSP video stream on.
     * @param username Username used to authenticate the stream.
     * @param password Password associated with the provided username.
     * @param callback Callback method called once the stream has started playing, or has irrecoverably failed.
     */
    public synchronized void initiatePlaybackOf(String url, SurgeSurface surface, String username, String password, final PlayerCallback callback) {
        initiatePlaybackOf(url, surface, username, password, null, null, callback);
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
     * @param surface The Surface that Surge will use to play the RTSP video stream on.
     * @param username Username used to authenticate the stream.
     * @param password Password associated with the provided username.
     * @param startTime If the RTSP stream supports recorded video, timestamp to start playing video from.
     * @param endTime If the RTSP stream supports recorded video, timestamp to finish playing video at.
     * @param callback Callback method called once the stream has started playing, or has irrecoverably failed.
     */
    public synchronized void initiatePlaybackOf(final String url, SurgeSurface surface, final String username, final String password, Date startTime, Date endTime, final PlayerCallback callback) {
        this.url = url;
        this.username = username;
        this.password = password;

        if (surface != null) {
            this.surface = surface;
        }

        logger.debug("Initating playback of {}", url);

        if (startTime != null) {
            rtspClient.setStartTime(startTime);
        }
        if (endTime != null) {
            rtspClient.setEndTime(endTime);
        }

        rtspClient.describe(url, username, password, rawResponse -> {
            DescribeResponse response = (DescribeResponse) rawResponse;

            if (response == null) {
                callback.response(RtspErrorCode.UNKNOWN_FAILURE);
                return;
            }

            if (response.getStatusCode() != RtspErrorCode.SUCCESS ||
                    response.getSessionDescriptions() == null ||
                    response.getSessionDescriptions().length == 0) {
                callback.response(response.getStatusCode());
                return;
            }

            for (SessionDescription sessionDescription : response.getSessionDescriptions()) {
                logger.debug(sessionDescription.toString());
            }

            setSessionDescriptions(response.getSessionDescriptions());
            if (sessionDescriptions.length > 0) {
                setupStream(selectPreferredSessionDescription(getSessionDescriptions()),
                        errorCode -> callback.response(errorCode));
            } else {
                throw new RuntimeException("No session description available, is the stream active?");
            }

            startFpsCounter();
        });
    }

    private void setupStream(SessionDescription sessionDescription, final PlayerCallback callback) {
        this.sessionDescription = sessionDescription;
        initialiseDecoder(sessionDescription);
        rtspClient.setup(sessionDescription, response -> {
            if (response == null) {
                callback.response(RtspErrorCode.UNKNOWN_FAILURE);
                return;

            } else if (response.getStatusCode() == RtspErrorCode.SUCCESS) {
                rtspClient.play();
            }

            callback.response(response.getStatusCode());
        });
    }

    protected void initialiseDecoder(SessionDescription sessionDescription) {

        System.out.println("Initializing new decoders");

        if (surface != null) {
            Decoder currentDecoder = decoder;
            decoder = null;

            if (sessionDescription.getType() == H264) {
                decoder = DecoderFactory.generateH264Decoder(surface.getSurface());
            } else if (sessionDescription.getType() == MP4V) {
                decoder = DecoderFactory.generateMP4VDecoder(surface.getSurface());
            } else if (sessionDescription.getType() == MJPEG) {
                decoder = DecoderFactory.generateMJPEGDecoder(surface.getSurface());
            }

            if (currentDecoder != null) {
                try {
                    currentDecoder.close();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * Resumes playback of a paused stream.
     */
    public void play() {
        logger.debug("Starting/resuming playback of {}", url);
        rtspClient.play();
    }

    /**
     * Pause playback of the stream.
     */
    public void pause() {
        logger.debug("Pausing playback of {}", url);
        rtspClient.pause();
    }

    /**
     *Stop playback of the stream, issuing the TEARDOWN RTSP request.
     * 
     * Once Stop has been requested, the stream cannot be restarted via the Play() command.
     * To restart a stream, please re-execute the InitiatePlaybackOf() command.
     */
    public void stop() {
        logger.debug("Stopping playback of {}", url);
        rtspClient.stopClient();
        stopFpsCounter();
        decoder = null;
    }

    /**
     * Sek playback to a specified point in time.
     *
     * Seeking has to be supported by the RTSP stream for this transaction to succeed. If unsupported,
     * calling this method will likely cause the stream to stop all playback and timeout.
     * @param startTime Timestamp to start playing video from.
     * @param endTime Timestamp to finish playing video at.
     */
    public void seek(Date startTime, Date endTime) {
        rtspClient.pause();

        if (startTime != null) {
            rtspClient.setStartTime(startTime);
        } else {
            rtspClient.clearStartTime();
        }

        if (endTime != null) {
            rtspClient.setEndTime(endTime);
        } else {
            rtspClient.clearEndTime();
        }

        rtspClient.play();
    }

    protected SessionDescription selectPreferredSessionDescription(SessionDescription[] sessionDescriptions) {
        return sessionDescriptions[0];
    }

    /**
     * The surface that Surge will use to play the RTSP video stream on.
     * @param surface Surface to play the RTSP video on.
     */
    public void setSurface(SurgeSurface surface) {
        this.surface = surface;

        if (sessionDescription != null) {
            initialiseDecoder(sessionDescription);
        } else {
            logger.error("No decoder generated as there are no available SessionDescriptions");
        }

    }

    @Override
    public void close() throws Exception {
        try {
            rtspClient.close();
            stopFpsCounter();
        } catch (Exception e) {
            System.out.println("Failed to close and clean up the native RTSP Client");
        }

        if (decoder != null) {
            decoder.close();
        }
    }

    @Override
    public void clientDidTimeout() {
        logger.error("RTSP client timed out - calling delegate");

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
                    surface.getWidth(),
                    surface.getHeight(),
                    presentationTime,
                    duration);
        }

        framesPerSecondCounter++;
    }

    @Override
    public void clientReceivedExtendedHeader(ByteBuffer buffer, int length) {
        System.out.println("Received extended header");
    }



    private int framesPerSecond = 0;
    private int framesPerSecondCounter = 0;
    private boolean fpsThreadIsRunning = false;


    private void startFpsCounter() {
        if (!fpsThreadIsRunning) {
            fpsThreadIsRunning = true;

            new Thread(() -> {
                while (fpsThreadIsRunning) {
                    framesPerSecond = framesPerSecondCounter;
                    framesPerSecondCounter = 0;
                    if (delegate != null) {
                        delegate.rtspPlayerDidUpdateFps(framesPerSecond);
                    }
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }).start();
        }
    }

    private void stopFpsCounter() {
        fpsThreadIsRunning = false;
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
     */
    public int getFramesPerSecond() {
        return framesPerSecond;
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
}
