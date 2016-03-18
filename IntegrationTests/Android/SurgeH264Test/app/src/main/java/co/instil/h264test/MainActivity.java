package co.instil.h264test;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Surface;
import android.view.TextureView;

import co.instil.h264test.co.instil.h264test.decoder.Decoder;
import co.instil.surge.rtsp.Surge;


public class MainActivity extends Activity implements TextureView.SurfaceTextureListener, RtspClientDelegate {

    static {
        Surge.SetLoggingDelegate(new RtspLogger());
    }

    private RtspClient client;
    private Decoder decoder;

    // UI Lifecycle -- -- --
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        this.decoder = new Decoder();

        ((TextureView) findViewById(R.id.textureView)).setSurfaceTextureListener(this);

        //this.client = new RtspClient(this, "rtsp://192.168.1.36:8554/test.264");
        this.client = new RtspClient(this, "rtsp://192.168.1.105:5454/camera.sdp");

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    // Texture View Life Cycle -- -- --
    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1) {
        this.decoder.startDecoder(new Surface(surfaceTexture));
        this.client.PlayAsync();
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
        try {
            this.decoder.stopDecoder();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {

    }

    // Rtsp Client Delegate
    @Override
    public void Timeout() {

    }

    @Override
    public void Payload(byte[] payload) {
        Log.i("TEST", "PAYLOAD");
        this.decoder.decodeFrameBuffer(payload);
    }
}
