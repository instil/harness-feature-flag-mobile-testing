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

package co.instil.surge.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;
import co.instil.surge.decoders.h264.nalu.NaluSegment;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Wrapper around the static initialisation methods of the {@link MediaCodec} class which
 * enables dependency injection of MediaCodec instances.
 */
public class MediaCodecFactory {

    public static final String H264_DECODER_IDENTIFIER = "video/avc";

    public MediaCodec createH264DecoderWithParameters(NaluSegment sps, NaluSegment pps, Surface surface, int width, int height) throws IOException {
        MediaCodec codec = MediaCodec.createDecoderByType(H264_DECODER_IDENTIFIER);
        MediaFormat format = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, 720, 480);
        format.setInteger(MediaFormat.KEY_WIDTH, width);
        format.setInteger(MediaFormat.KEY_HEIGHT, height);
        format.setByteBuffer("csd-0", ByteBuffer.wrap(sps.getPayload()));
        format.setByteBuffer("csd-1", ByteBuffer.wrap(pps.getPayload()));
        format.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 0);
        codec.configure(format, surface, null, 0);
        return codec;
    }

    public MediaCodec createDecoderOfType(String type) throws IOException {
        return MediaCodec.createDecoderByType(type);
    }

    public MediaCodec createEncoderOfType(String type) throws IOException {
        return MediaCodec.createEncoderByType(type);
    }

}
