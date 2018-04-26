/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surgeandroiddemoapp;

import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import co.instil.surge.callbacks.PlayerCallback;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.player.RtspErrorCode;
import co.instil.surge.player.SurgeRtspPlayer;
import co.instil.surge.client.SurgeSurface;
import co.instil.surge.player.SurgeRtspPlayerDelegate;

/**
 *
 */
public class MainActivity extends AppCompatActivity implements TextureView.SurfaceTextureListener, SurgeRtspPlayerDelegate {

    private static Logger logger = LoggerFactory.getLogger(MainActivity.class);

    private final SurgeRtspPlayer player = new SurgeRtspPlayer();
    private int currentPaletteIndex = 0;
    private TextureView textureView;

    private static final Calendar calendar = Calendar.getInstance();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textureView = (TextureView) findViewById(R.id.textureView);
        textureView.setSurfaceTextureListener(this);
        player.delegate = this;
        calendar.setTimeZone(TimeZone.getTimeZone("Europe/London"));
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width, int height) {
        logger.debug("textureView.isAvailable: {}", textureView.isAvailable());

        SurgeSurface surface = new SurgeSurface(new Surface(surfaceTexture), width, height);

        player.initiatePlaybackOf("rtsp://192.168.1.52:8554/test",
                surface,
                new PlayerCallback() {
                    @Override
                    public void response(RtspErrorCode errorCode) {
                        System.out.println("Finished starting stream");
                    }
                });
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        player.stop();
        return false;
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    }

    public void seekBackAnHour(View view) {
        logger.debug("Seeking back an hour");

        calendar.add(Calendar.HOUR, -1);
        Date timeAnHourago = calendar.getTime();

        player.seek(timeAnHourago, null);
    }

    @Override
    public void rtspPlayerDidTimeout() {

    }

    @Override
    public void rtspPlayerDidUpdateFps(int fps) {
        logger.debug("Updated fps: " + fps);
    }
}
