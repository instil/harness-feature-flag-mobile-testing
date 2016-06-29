package co.instil.surge.decoders;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.view.Surface;

import java.nio.ByteBuffer;

/**
 * Decoder which renders a MJPEG stream to a {@link android.view.Surface}.
 */
public class MjpegDecoder implements Decoder {

    private final Surface surface;

    public MjpegDecoder(Surface surface) {
        this.surface = surface;
    }

    @Override
    public void decodeFrameBuffer(ByteBuffer frameBuffer, int duration, int presentationTime) {
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
