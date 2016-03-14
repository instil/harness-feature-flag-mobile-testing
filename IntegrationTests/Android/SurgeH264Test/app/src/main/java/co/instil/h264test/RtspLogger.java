package co.instil.h264test;

import android.util.Log;

import co.instil.surge.rtsp.LoggingDelegate;

/**
 * Created by redbrain on 10/03/2016.
 */
public class RtspLogger extends LoggingDelegate {

    private String TAG = "SURGE-RTSP";

    public void info(String message) {
        Log.i(TAG, message);
    }

    public void error(String message) {
        Log.e(TAG, message);
    }

    public void debug(String message) {
        Log.d(TAG,message);
    }

    public void fatal(String message) {
        Log.wtf(TAG, message);
    }

    public void warn(String message) {
        Log.w(TAG, message);
    }

    public void trace(String message) {
        Log.d(TAG, message);
    }

}
