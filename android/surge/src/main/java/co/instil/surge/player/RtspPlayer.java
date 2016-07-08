package co.instil.surge.player;

import android.view.Surface;
import co.instil.surge.client.DescribeResponse;
import co.instil.surge.client.RtspClient;
import co.instil.surge.client.RtspClientDelegate;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.h264.AsyncH264Decoder;
import co.instil.surge.decoders.mp4v.AsyncMp4vDecoder;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.decoders.mjpeg.MjpegDecoder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.nio.ByteBuffer;

import static co.instil.surge.client.SessionType.H264;
import static co.instil.surge.client.SessionType.MJPEG;
import static co.instil.surge.client.SessionType.MP4V;

/**
 *
 */
public class RtspPlayer implements AutoCloseable, RtspClientDelegate {

    private static Logger logger = LoggerFactory.getLogger(RtspPlayer.class);

    private final RtspClient rtspClient;

    private String url;
    private Surface surface;
    private Decoder decoder;
    private SessionDescription sessionDescription;

    public RtspPlayer() {
        rtspClient = new RtspClient(this);
    }

    public void initiatePlaybackOf(String url, Surface surface) {
        this.url = url;
        this.surface = surface;
        logger.debug("Initating playback of {}", url);

        DescribeResponse response = rtspClient.describe(url);
        for (SessionDescription sessionDescription : response.getSessionDescriptions()) {
            logger.debug(sessionDescription.toString());
        }

        SessionDescription[] sessionDescriptions = response.getSessionDescriptions();
        if (sessionDescriptions.length > 0) {
            setupStream(sessionDescriptions[0]);
        } else {
            throw new RuntimeException("No session description available, is the stream active?");
        }
    }

    private void setupStream(SessionDescription sessionDescription) {
        this.sessionDescription = sessionDescription;
        initialiseDecoder(sessionDescription);
        rtspClient.setup(sessionDescription);
        rtspClient.play();
    }

    private void initialiseDecoder(SessionDescription sessionDescription) {
        if (sessionDescription.getType() == H264) {
            decoder = new AsyncH264Decoder(surface);
        } else if (sessionDescription.getType() == MP4V) {
            decoder = new AsyncMp4vDecoder(surface);
        } else if (sessionDescription.getType() == MJPEG) {
            decoder = new MjpegDecoder(surface);
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

    @Override
    public void close() throws Exception {
        rtspClient.close();
        decoder.close();
    }

    @Override
    public void clientDidTimeout() {
        logger.error("RTSP client timed out");
    }

    @Override
    public void clientReceivedFrame(ByteBuffer byteBuffer,
                                    int width,
                                    int height,
                                    int presentationTime,
                                    int duration) {

        decoder.decodeFrameBuffer(sessionDescription,
                                  byteBuffer,
                                  width,
                                  height,
                                  presentationTime,
                                  duration);
    }

}
