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

import android.view.Surface;

import co.instil.surge.decoders.h264.AsyncH264Decoder;
import co.instil.surge.decoders.h264.SyncH264Decoder;
import co.instil.surge.decoders.mjpeg.MjpegDecoder;
import co.instil.surge.decoders.mp4v.AsyncMp4vDecoder;
import co.instil.surge.device.DeviceExaminer;

/*
 * Factory to generate each supported type of decoder.
 */
public class DecoderFactory {

    private static DeviceExaminer deviceExaminer = new DeviceExaminer();

    private DecoderFactory() {}

    public static Decoder generateH264Decoder(Surface surface) {
        if (deviceExaminer.isPreLollipopDevice()) {
            System.out.println("Detecting Lollipop decoder");
            return new SyncH264Decoder(surface);
        }

        return new AsyncH264Decoder(surface);
    }

    public static Decoder generateMP4VDecoder(Surface surface) {
        if (deviceExaminer.isPreLollipopDevice()) {
            throw new UnsupportedOperationException("No Mp4V decoder available for API 19");
        }

        return new AsyncMp4vDecoder(surface);
    }

    public static Decoder generateMJPEGDecoder(Surface surface) {
        return new MjpegDecoder(surface);
    }
}
