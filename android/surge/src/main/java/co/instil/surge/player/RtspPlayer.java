package co.instil.surge.player;

import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;

import co.instil.surge.client.DescribeResponse;
import co.instil.surge.client.RtspClient;
import co.instil.surge.client.RtspClientDelegate;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.decoders.H264Decoder;
import co.instil.surge.decoders.MjpegDecoder;
import co.instil.surge.decoders.Mp4vDecoder;
import co.instil.surge.logger.Logger;

import static co.instil.surge.client.SessionType.H264;
import static co.instil.surge.client.SessionType.MJPEG;
import static co.instil.surge.client.SessionType.MP4V;

/**
 *
 */
public class RtspPlayer implements AutoCloseable, RtspClientDelegate {

    private final RtspClient rtspClient;

    private String url;
    private Surface surface;
    private Decoder decoder;

    public RtspPlayer() {
        rtspClient = new RtspClient(this);
    }

    public void initiatePlaybackOf(String url, Surface surface) {
        this.url = url;
        this.surface = surface;
        Logger.debug("Initating playback of " + url);

        DescribeResponse response = rtspClient.describe(url);
        for (SessionDescription sessionDescription : response.getSessionDescriptions()) {
            Logger.debug(sessionDescription.toString());
        }

        SessionDescription[] sessionDescriptions = response.getSessionDescriptions();
        if (sessionDescriptions.length > 0) {
            setupStream(sessionDescriptions[0]);
        } else {
            throw new RuntimeException("No session descriptions, is the Stream up?");
        }
    }

    private void setupStream(SessionDescription sessionDescription) {
        initialiseDecoder(sessionDescription);
        rtspClient.setup(sessionDescription);
        rtspClient.play();
    }

    private void initialiseDecoder(SessionDescription sessionDescription) {
        if (sessionDescription.getType() == H264) {
            decoder = new H264Decoder(surface);
        } else if (sessionDescription.getType() == MP4V) {
            decoder = new Mp4vDecoder(surface);
        } else if (sessionDescription.getType() == MJPEG) {
            decoder = new MjpegDecoder();
        }
    }

    public void play() {
        Logger.debug("Starting/resuming playback of {}" + url);
        rtspClient.play();
    }

    public void pause() {
        Logger.debug("Pausing playback of {}" + url);
        rtspClient.pause();
    }

    public void stop() {
        Logger.debug("Stopping playback of {}" + url);
        rtspClient.tearDown();
    }

    @Override
    public void close() throws Exception {
        rtspClient.close();
        decoder.close();
    }

    @Override
    public void clientDidTimeout() {
        Logger.error("RTSP client timed out");
    }

    @Override
    public void clientReceivedFrame(ByteBuffer byteBuffer) {
        decoder.decodeFrameBuffer(byteBuffer, 0, 0);
    }

}
