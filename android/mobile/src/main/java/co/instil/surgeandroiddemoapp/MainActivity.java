package co.instil.surgeandroiddemoapp;

import android.graphics.SurfaceTexture;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Surface;
import android.view.TextureView;

import co.instil.surge.player.RtspPlayer;

public class MainActivity extends AppCompatActivity implements TextureView.SurfaceTextureListener {

    private final RtspPlayer player = new RtspPlayer();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ((TextureView) findViewById(R.id.textureView)).setSurfaceTextureListener(this);
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width, int height) {
        player.initiatePlaybackOf("rtsp://192.168.1.133:8554/test", new Surface(surfaceTexture));
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        player.stop();
        return false;
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {}

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {}
}
