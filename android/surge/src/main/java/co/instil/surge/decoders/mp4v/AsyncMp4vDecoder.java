/*
 * Copyright (c) 2016 Instil Software.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

package co.instil.surge.decoders.mp4v;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaCodec.CodecException;
import android.media.MediaFormat;
import android.view.Surface;
import co.instil.surge.client.SessionDescription;
import co.instil.surge.decoders.Decoder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

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

    private static Logger logger = LoggerFactory.getLogger(AsyncMp4vDecoder.class);

    private MediaCodec mediaCodec;
    private MediaFormat mediaFormat;
    private Surface surface;
    private List<ByteBuffer> decodeQueue = new ArrayList<>();
    private List<Integer> availableInputBuffers = new ArrayList<>();

    public AsyncMp4vDecoder(Surface surface) {
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
            logger.debug("Received MP4V frame for decoding");

            if (mediaCodec == null) {
                mediaCodec = createMediaCodec(sessionDescription, width, height);
                mediaCodec.setCallback(this);
                mediaCodec.start();
            }

            if (availableInputBuffers.size() > 0) {
                submitFrameToBuffer(frameBuffer, presentationTime, availableInputBuffers.remove(0), mediaCodec);
            } else {
                logger.debug("No input buffer available, queueing frame until one becomes free");
                decodeQueue.add(frameBuffer);
            }

        } catch (IllegalStateException e) {
            logger.error("Failed to get an input buffer from the media codec", e);
        } catch (IOException e) {
            logger.error("Device does not support MP4V decoding", e);
            throw new RuntimeException("Device doesn't support MP4V decoding");
        } catch (Exception e) {
            logger.error("Failed to decode MP4V", e);
        }
    }

    private MediaCodec createMediaCodec(SessionDescription sessionDescription, int width, int height) throws IOException {
        mediaCodec = MediaCodec.createDecoderByType(MediaFormat.MIMETYPE_VIDEO_MPEG4);
        mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_MPEG4, width, height);
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
        mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(sessionDescription.getDecoderConfig().getBytes(Charset.forName("UTF-8"))));
        mediaCodec.configure(mediaFormat, surface, null, 0);
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
        logger.debug("Submitting frame to input buffer {}", bufferIndex);
        ByteBuffer inputBuffer = codec.getInputBuffer(bufferIndex);

        byte[] frame = new byte[frameBuffer.capacity()];
        frameBuffer.get(frame);
        inputBuffer.put(frame);

        int flags = 0;
        codec.queueInputBuffer(bufferIndex, 0, frame.length, presentationTime, flags);
    }

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {
        logger.debug("Input buffer {} available", index);
        if (decodeQueue.size() == 0) {
            availableInputBuffers.add(index);
            return;
        }
        ByteBuffer frame = decodeQueue.remove(0);
        submitFrameToBuffer(frame, 1, index, codec);
    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, BufferInfo info) {
        logger.debug("Rendering and releasing output buffer");
        codec.releaseOutputBuffer(index, true);
    }

    @Override
    public void onError(MediaCodec codec, CodecException error) {
        logger.debug("MP4V decoding failed", error);
        if (!error.isRecoverable()) {
            resetCodec();
        }
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
        logger.debug("Output format changed");
    }
}
