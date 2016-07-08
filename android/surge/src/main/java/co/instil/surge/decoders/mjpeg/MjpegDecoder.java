package co.instil.surge.decoders.mjpeg;

import android.annotation.TargetApi;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.view.Surface;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.Decoder;

import java.nio.ByteBuffer;

/**
 * Decoder which renders a MJPEG stream to a {@link android.view.Surface}.
 */
@TargetApi(19)
public class MjpegDecoder implements Decoder {

    private final Surface surface;

    public MjpegDecoder(Surface surface) {
        this.surface = surface;
    }

    @Override
    public void decodeFrameBuffer(SessionDescription sessionDescription,
                                  ByteBuffer frameBuffer,
                                  int width,
                                  int height,
                                  int presentationTime,
                                  int duration) {

        byte[] imageBytes = new byte[frameBuffer.limit()];
        frameBuffer.get(imageBytes);
        Bitmap bitmap = BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
        Canvas canvas = surface.lockCanvas(null);
        canvas.drawBitmap(bitmap, 0, 0, null);
        surface.unlockCanvasAndPost(canvas);
    }

    @Override
    public void close() throws InterruptedException {}

}
