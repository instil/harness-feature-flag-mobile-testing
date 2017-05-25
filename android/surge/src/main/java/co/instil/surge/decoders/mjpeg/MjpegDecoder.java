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
import android.view.Surface;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.nio.ByteBuffer;

import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.Decoder;

/**
 * Decoder which renders a MJPEG stream to a {@link android.view.Surface}.
 */
@TargetApi(19)
public class MjpegDecoder implements Decoder {

    private static Logger logger = LoggerFactory.getLogger(MjpegDecoder.class);

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

        try {
            byte[] imageBytes = new byte[frameBuffer.limit()];
            frameBuffer.get(imageBytes);
            Bitmap bitmap = BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
            if (bitmap != null) {
                Canvas canvas = surface.lockCanvas(null);
                canvas.drawBitmap(bitmap,
                        new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()),
                        new Rect(0, 0, width, height),
                        null);
                surface.unlockCanvasAndPost(canvas);
            }
        } catch (Exception e) {
            logger.error("Failed to decode frame", e);
        }
    }

    @Override
    public void close() throws InterruptedException {}

}
