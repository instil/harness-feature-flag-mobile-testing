/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
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
