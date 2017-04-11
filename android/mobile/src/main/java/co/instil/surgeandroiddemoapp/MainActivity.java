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

import co.instil.surge.client.SessionDescription;
import co.instil.surge.player.RtspPlayer;
import co.instil.surge.player.RtspPlayerDelegate;

/**
 *
 */
public class MainActivity extends AppCompatActivity implements TextureView.SurfaceTextureListener, RtspPlayerDelegate {

    private static Logger logger = LoggerFactory.getLogger(MainActivity.class);

    private final RtspPlayer player = new RtspPlayer();
    private SessionDescription[] palettes;
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
        player.initiatePlaybackOf("rtsp://192.168.1.104:8554/test", new Surface(surfaceTexture));
        palettes = player.getSessionDescriptions();
        System.out.println("Finished starting stream");
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

    public void iteratePalette(View view) {
        currentPaletteIndex = ++currentPaletteIndex % palettes.length;
        System.out.println(palettes[currentPaletteIndex].getType());
//        player.changePalette(palettes[currentPaletteIndex]);
        palettes = player.getSessionDescriptions();
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
//        SessionDescription descrip = palettes[currentPaletteIndex];
//        logger.debug("Updated fps: " + fps + "/" + descrip.getFramerate());
    }
}
