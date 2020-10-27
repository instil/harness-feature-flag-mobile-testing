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
import android.os.HandlerThread;
import android.view.Surface;

import java.nio.ByteBuffer;

import co.instil.surge.client.SessionDescription;
import co.instil.surge.client.SurgeVideoView;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.diagnostics.DiagnosticsTracker;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

/**
 * Decoder which renders a MJPEG stream to a {@link android.view.TextureView}.
 */
@TargetApi(19)
public class MjpegDecoder implements Decoder {
    private static final Logger LOGGER = LoggerFactory.getLogger(MjpegDecoder.class);

    private final SurgeVideoView videoView;
    private final Surface surface;
    @SuppressWarnings("PMD.SingularField")
    private final HandlerThread decoderThread;
    private final DiagnosticsTracker diagnostics;

    public MjpegDecoder(SurgeVideoView videoView, DiagnosticsTracker diagnosticsTracker) {
        this.videoView = videoView;
        this.surface = videoView.generateUniqueSurface();
        this.diagnostics = diagnosticsTracker;
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

        diagnostics.trackNewFrameOfSize(frameBuffer.remaining());

        try {
            final ByteBuffer test = deepCopy(frameBuffer, null);
            byte[] imageBytes = new byte[test.limit()];
            test.get(imageBytes);
            final Bitmap bitmap = BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
            if (bitmap != null) {
                configureSurfaceForNewVideoDimensions(bitmap.getWidth(), bitmap.getHeight());
                trackChangeInStreamDimensionsIfRequired(bitmap);

                Canvas canvas = surface.lockCanvas(null);
                canvas.drawBitmap(bitmap,
                        new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()),
                        new Rect(0, 0, width, height),
                        null);
                surface.unlockCanvasAndPost(canvas);
            }
        } catch (Exception e) {
            LOGGER.error("Failed to decode frame", e);
        }
    }

    private void configureSurfaceForNewVideoDimensions(int width, int height) {
        videoView.setVideoDimensions(width, height);
    }

    private void trackChangeInStreamDimensionsIfRequired(Bitmap bitmap) {
        diagnostics.trackNewFrameDimensions(bitmap.getWidth(), bitmap.getHeight());
    }

    @Override
    public void close() {
        if (surface != null) {
            surface.release();
        }
    }

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
