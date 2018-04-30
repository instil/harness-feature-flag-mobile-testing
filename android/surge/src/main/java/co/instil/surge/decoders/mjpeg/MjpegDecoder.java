/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mjpeg;

import android.annotation.TargetApi;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.Surface;

import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

import java.nio.ByteBuffer;

/**
 * Decoder which renders a MJPEG stream to a {@link android.view.Surface}.
 */
@TargetApi(19)
public class MjpegDecoder implements Decoder {

    private static Logger logger = LoggerFactory.getLogger(MjpegDecoder.class);

    private final Surface surface;
    private HandlerThread decoderThread;

    public MjpegDecoder(Surface surface) {
        this.surface = surface;
        decoderThread = new HandlerThread("decoderThread");
        decoderThread.start();
    }

    @Override
    public void decodeFrameBuffer(SessionDescription sessionDescription,
                                  ByteBuffer frameBuffer,
                                  final int width,
                                  final int height,
                                  int presentationTime,
                                  int duration) {

        try {
            final ByteBuffer test = deepCopy(frameBuffer, null);
            new Handler(decoderThread.getLooper()).post(new Runnable() {
                @Override
                public void run() {
                    byte[] imageBytes = new byte[test.limit()];
                    test.get(imageBytes);
                    final Bitmap bitmap = BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
                    if (bitmap != null) {
                        Canvas canvas = surface.lockCanvas(null);
                        canvas.drawBitmap(bitmap,
                                new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()),
                                new Rect(0, 0, width, height),
                                null);
                        surface.unlockCanvasAndPost(canvas);
                    }
                }
            });
        } catch (Exception e) {
            logger.error("Failed to decode frame", e);
        }
    }

    @Override
    public void close() throws InterruptedException {}

    private ByteBuffer deepCopy(ByteBuffer source, ByteBuffer target) {

        int sourceP = source.position();
        int sourceL = source.limit();

        if (null == target) {
            target = ByteBuffer.allocate(source.remaining());
        }
        target.put(source);
        target.flip();

        source.position(sourceP);
        source.limit(sourceL);
        return target;
    }

}
