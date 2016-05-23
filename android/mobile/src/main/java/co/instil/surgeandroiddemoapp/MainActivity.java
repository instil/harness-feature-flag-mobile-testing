package co.instil.surgeandroiddemoapp;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import co.instil.surge.player.RtspPlayer;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        RtspPlayer player = new RtspPlayer();
        player.initiatePlaybackOf("rtsp://192.168.1.133:8554/test");
    }
}
