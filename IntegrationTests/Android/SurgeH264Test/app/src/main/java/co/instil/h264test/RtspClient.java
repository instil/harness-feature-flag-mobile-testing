package co.instil.h264test;

import co.instil.surge.rtsp.RtspPalette;
import co.instil.surge.rtsp.RtspSessionDescriptions;
import co.instil.surge.rtsp.Surge;

/**
 * Created by redbrain on 10/03/2016.
 */
public class RtspClient extends co.instil.surge.rtsp.RtspClientDelegate implements Runnable {

    private String url;
    private RtspClientDelegate delegate;
    private RtspSessionDescriptions palettes;

    private co.instil.surge.rtsp.RtspClientWrapper client;

    public RtspClient(RtspClientDelegate delegate, String url) {
        this.url = url;
        this.delegate = delegate;
        this.client = Surge.CreateNewRtspClient(this);
    }

    // wrapper for commands...
    private int describe() {
        palettes = new RtspSessionDescriptions();
        return Surge.Describe(client, url, false, "", "", palettes);
    }

    private int setup () {
        RtspPalette[] p = palettes.getPaletteEntries();
        return Surge.Setup(client, p[0]);
    }

    private int play() {
        return Surge.Play(client);
    }

    // Async
    public void PlayAsync() {
        new Thread(this).start();
    }

    // DELEGATE
    public void ClientDidTimeout() {
        if (delegate != null) {
            delegate.Timeout();
        }
    }

    public void Payload(byte[] payload) {
        if (delegate != null) {
            delegate.Payload(payload);
        }
    }

    // RUNNABLE
    @Override
    public void run() {
        StartStream(3);
    }

    // Start
    private void StartStream(int retryCount) {
        boolean should_retry = true;
        do {
            int retval = describe();
            if (retval == 0) {
                retval = setup();
                if (retval == 0) {
                    retval = play();

                    // stop the retry loop
                    if (retval == 0) {
                        should_retry = false;
                    }
                }
            }
            retryCount--;
        } while (should_retry && retryCount > 0);
    }
}
