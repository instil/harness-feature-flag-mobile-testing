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

package co.instil.surge.player;

import android.view.Surface;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.nio.ByteBuffer;
import java.util.Date;

import co.instil.surge.callbacks.PlayerCallback;
import co.instil.surge.client.DescribeResponse;
import co.instil.surge.client.ExtendedHeader;
import co.instil.surge.client.RtspClient;
import co.instil.surge.client.RtspClientDelegate;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.decoders.DecoderFactory;

import static co.instil.surge.client.SessionType.H264;
import static co.instil.surge.client.SessionType.MJPEG;
import static co.instil.surge.client.SessionType.MP4V;

/**
 *
 */
public class RtspPlayer implements AutoCloseable, RtspClientDelegate {

    private static Logger logger = LoggerFactory.getLogger(RtspPlayer.class);

    protected final RtspClient rtspClient;

    protected String url;
    protected String username;
    protected String password;
    protected SessionDescription[] sessionDescriptions;
    protected SessionDescription sessionDescription;

    protected ExtendedHeader extendedHeader;

    public RtspPlayerDelegate delegate;

    private Surface surface = null;
    private Decoder decoder;


    public RtspPlayer() {
        rtspClient = generateRtspClient();
    }

    protected RtspClient generateRtspClient() {
        return new RtspClient(this);
    }

    public void initiatePlaybackOf(String url, Surface surface, final PlayerCallback callback) {
        initiatePlaybackOf(url, surface, "", "", null, null, callback);
    }

    public void initiatePlaybackOf(String url, Surface surface, String username, String password, final PlayerCallback callback) {
        initiatePlaybackOf(url, surface, username, password, null, null, callback);
    }

    public void initiatePlaybackOf(String url, Surface surface, String username, String password, Date startTime, Date endTime, final PlayerCallback callback) {
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
            DescribeResponse response = (DescribeResponse)rawResponse;

            if (response == null ||
                    response.getSessionDescriptions() == null ||
                    response.getSessionDescriptions().length == 0) {
                callback.response(false);
                return;
            }

            for (SessionDescription sessionDescription : response.getSessionDescriptions()) {
                logger.debug(sessionDescription.toString());
            }

            setSessionDescriptions(response.getSessionDescriptions());
            if (sessionDescriptions.length > 0) {
                setupStream(selectPreferredSessionDescription(getSessionDescriptions()),
                        result -> callback.response(true));
            } else {
                throw new RuntimeException("No session description available, is the stream active?");
            }

            startFpsCounter();
        });
    }

    private void setupStream(SessionDescription sessionDescription, final PlayerCallback callback) {
        this.sessionDescription = sessionDescription;
        initialiseDecoder(sessionDescription);
        rtspClient.setup(sessionDescription, rawResponse -> {
            rtspClient.play();
            callback.response(true);
        });
    }

    private void initialiseDecoder(SessionDescription sessionDescription) {

        System.out.println("Initializing new decoders");

        if (surface != null) {
            if (decoder != null) {
                try {
                    decoder.close();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            if (sessionDescription.getType() == H264) {
                decoder = DecoderFactory.generateH264Decoder(surface);
            } else if (sessionDescription.getType() == MP4V) {
                decoder = DecoderFactory.generateMP4VDecoder(surface);
            } else if (sessionDescription.getType() == MJPEG) {
                decoder = DecoderFactory.generateMJPEGDecoder(surface);
            }
        }
    }

    public void play() {
        logger.debug("Starting/resuming playback of {}", url);
        rtspClient.play();
    }

    public void pause() {
        logger.debug("Pausing playback of {}", url);
        rtspClient.pause();
    }

    public void stop() {
        logger.debug("Stopping playback of {}", url);
        rtspClient.tearDown();
    }

    public void seek(Date startTime, Date endTime) {
        rtspClient.pause();

        if (startTime != null) {
            rtspClient.setStartTime(startTime);
        }

        if (endTime != null) {
            rtspClient.setEndTime(endTime);
        }

        rtspClient.play();
    }

    protected SessionDescription selectPreferredSessionDescription(SessionDescription[] sessionDescriptions) {
        return sessionDescriptions[0];
    }

    public void setSurface(Surface surface) {
        this.surface = surface;

        if (sessionDescription != null) {
            initialiseDecoder(sessionDescription);
        }

    }

    @Override
    public void close() throws Exception {
        try {
            rtspClient.close();
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
                    width,
                    height,
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


    private void startFpsCounter() {
        new Thread(() -> {
            while (true) {
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



    public SessionDescription getCurrentSessionDescription() {
        return sessionDescription;
    }

    public SessionDescription[] getSessionDescriptions() {
        return sessionDescriptions;
    }

    protected void setSessionDescriptions(SessionDescription[] sessionDescriptions) {
        this.sessionDescriptions = sessionDescriptions;
    }

    public ExtendedHeader getExtendedHeader() {
        return extendedHeader;
    }

    public int getFramesPerSecond() {
        return framesPerSecond;
    }
}
