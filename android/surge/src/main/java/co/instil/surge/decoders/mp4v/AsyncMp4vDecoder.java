/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders.mp4v;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaCodec.CodecException;
import android.media.MediaFormat;
import android.view.Surface;

import co.instil.surge.client.SessionDescription;
import co.instil.surge.client.SurgeVideoView;
import co.instil.surge.decoders.Decoder;
import co.instil.surge.diagnostics.DiagnosticsTracker;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

/**
 *
 */
@TargetApi(21)
public class AsyncMp4vDecoder extends MediaCodec.Callback implements Decoder {

    private static final Logger LOGGER = LoggerFactory.getLogger(AsyncMp4vDecoder.class);
    private static final String MEDIA_FORMAT_WIDTH_KEY = "width";
    private static final String MEDIA_FORMAT_HEIGHT_KEY = "height";

    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private SurgeVideoView videoView;
    private Surface surface;
    private List<ByteBuffer> decodeQueue = new ArrayList<>();
    private List<Integer> availableInputBuffers = new ArrayList<>();
    private DiagnosticsTracker diagnostics;

    public AsyncMp4vDecoder(SurgeVideoView videoView, DiagnosticsTracker diagnosticsTracker) {
        this.videoView = videoView;
        this.surface = videoView.generateUniqueSurface();
        this.diagnostics = diagnosticsTracker;
    }

    @Override
    public void decodeFrameBuffer(SessionDescription sessionDescription,
                                  ByteBuffer frameBuffer,
                                  int width,
                                  int height,
                                  int presentationTime,
                                  int duration) {
        diagnostics.trackNewFrameOfSize(frameBuffer.remaining() * 8);

        try {
            LOGGER.debug("Received MP4V frame for decoding");

            if (mediaCodec == null) {
                mediaCodec = createMediaCodec(sessionDescription, width, height);
                mediaCodec.setCallback(this);
                mediaCodec.start();
            }

            if (!availableInputBuffers.isEmpty()) {
                submitFrameToBuffer(frameBuffer, presentationTime, availableInputBuffers.remove(0), mediaCodec);
            } else {
                LOGGER.debug("No input buffer available, queueing frame until one becomes free");
                decodeQueue.add(frameBuffer);
            }

        } catch (IllegalStateException e) {
            LOGGER.error("Failed to get an input buffer from the media codec", e);
        } catch (IOException e) {
            LOGGER.error("Device does not support MP4V decoding", e);
            throw new UnsupportedOperationException("Device doesn't support MP4V decoding", e);
        } catch (Exception e) {
            LOGGER.error("Failed to decode MP4V", e);
        }
    }

    private MediaCodec createMediaCodec(SessionDescription sessionDescription, int width, int height) throws IOException {
        mediaCodec = MediaCodec.createDecoderByType(MediaFormat.MIMETYPE_VIDEO_MPEG4);
        mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_MPEG4, width, height);
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
        mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(sessionDescription.getDecoderConfig().getBytes(Charset.forName("UTF-8"))));
        mediaCodec.configure(mediaFormat, videoView.generateUniqueSurface(), null, 0);
        return mediaCodec;
    }

    private void resetCodec() {
        availableInputBuffers.clear();
        mediaCodec.reset();
        mediaCodec.configure(mediaFormat, surface, null, 0);
        mediaCodec.setCallback(this);
        mediaCodec.start();
    }

    @Override
    public void close() throws InterruptedException {
        if (mediaCodec != null) {
            mediaCodec.stop();
            mediaCodec.release();
        }
    }

    private void submitFrameToBuffer(ByteBuffer frameBuffer, int presentationTime, int bufferIndex, MediaCodec codec) {
        LOGGER.debug("Submitting frame to input buffer {}", bufferIndex);
        ByteBuffer inputBuffer = codec.getInputBuffer(bufferIndex);

        byte[] frame = new byte[frameBuffer.capacity()];
        frameBuffer.get(frame);
        inputBuffer.put(frame);

        int flags = 0;
        codec.queueInputBuffer(bufferIndex, 0, frame.length, presentationTime, flags);
    }

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {
        LOGGER.debug("Input buffer {} available", index);
        if (decodeQueue.isEmpty()) {
            availableInputBuffers.add(index);
            return;
        }
        ByteBuffer frame = decodeQueue.remove(0);

        try {
            submitFrameToBuffer(frame, 1, index, codec);
        } catch (Exception e) {
            LOGGER.error("Failed to decode MP4V frame: " + e.toString());
            LOGGER.printStackTrace(e);
        }
    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, BufferInfo info) {
        LOGGER.debug("Rendering and releasing output buffer");
        codec.releaseOutputBuffer(index, true);
    }

    @Override
    public void onError(MediaCodec codec, CodecException error) {
        LOGGER.debug("MP4V decoding failed", error);
        if (!error.isRecoverable()) {
            resetCodec();
        }
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
        mediaFormat = format;
        updateVideoPlayerForNewVideoDimensions();
    }

    private void updateVideoPlayerForNewVideoDimensions() {
        if (mediaFormatHasStreamDimensions()) {
            int streamWidth = mediaFormat.getInteger(MEDIA_FORMAT_WIDTH_KEY);
            int streamHeight = mediaFormat.getInteger(MEDIA_FORMAT_HEIGHT_KEY);
            videoView.setVideoDimensions(streamWidth, streamHeight);
            diagnostics.trackNewFrameDimensions(streamWidth, streamHeight);
        }
    }

    private boolean mediaFormatHasStreamDimensions() {
        return mediaFormat.containsKey(MEDIA_FORMAT_WIDTH_KEY) && mediaFormat.containsKey(MEDIA_FORMAT_HEIGHT_KEY);
    }
}
