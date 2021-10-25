/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surgeandroiddemoapp;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.EditText;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import co.instil.surge.player.SurgeRtspPlayer;
import co.instil.surge.player.SurgeRtspPlayerDelegate;

/**
 *
 */
public class MainActivity extends AppCompatActivity implements TextureView.SurfaceTextureListener, SurgeRtspPlayerDelegate {

    private static Logger logger = LoggerFactory.getLogger(MainActivity.class);

    private TextureView textureView;

    private final SurgeRtspPlayer player = new SurgeRtspPlayer();
    private boolean isPlaying = false;
    private boolean isPaused = false;

    private static final Calendar calendar = Calendar.getInstance();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textureView = findViewById(R.id.textureView);
        textureView.setSurfaceTextureListener(this);

        player.delegate = this;
        player.setInterleavedTransport(true);

        player.setTlsCertificateValidationEnabled(false);
        player.setTlsSelfSignedCertificatesAllowed(true);

        calendar.setTimeZone(TimeZone.getTimeZone("Europe/London"));
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width, int height) {
        logger.debug("textureView.isAvailable: {}", textureView.isAvailable());

        drawSolidColorOnSurfaceTexture(Color.BLACK, surfaceTexture);

        player.setPlayerView(textureView);
    }

    public void startStream(View view) {
        if (isPlaying) {
            player.stop();
        }

        isPlaying = true;

        EditText urlTextField = findViewById(R.id.url);
        String url = urlTextField.getText().toString();

        player.initiatePlaybackOf(url,
                errorCode -> System.out.println("Finished starting stream"));
    }

    public void pauseStream(View view) {
        if (isPlaying && !isPaused) {
            isPaused = true;
            player.pause();
        }
    }

    public void playStream(View view) {
        if (isPlaying && isPaused) {
            isPaused = false;
            player.play();
        }
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        isPlaying = false;
        player.stop();
        return false;
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    }

    @Override
    public void rtspPlayerDidTimeout() {

    }

    @Override
    public void rtspPlayerDidUpdateFps(int fps) {
        logger.debug("Updated fps: " + fps);
    }

    private void seekBackAnHour(View view) {
        logger.debug("Seeking back an hour");

        calendar.add(Calendar.HOUR, -1);
        Date timeAnHourago = calendar.getTime();

        player.seek(timeAnHourago, null);
    }

    private void drawSolidColorOnSurfaceTexture(int color, SurfaceTexture surfaceTexture) {
        Surface surface = new Surface(surfaceTexture);
        Canvas canvas = surface.lockCanvas(null);
        Paint painter = new Paint();
        painter.setColor(color);
        painter.setStyle(Paint.Style.FILL);
        canvas.drawRect(0, 0, 0, 0, painter);
        surface.unlockCanvasAndPost(canvas);
        surface.release();
    }
}
