/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.decoders;

import co.instil.surge.client.SurgeVideoView;
import co.instil.surge.decoders.h264.AsyncH264Decoder;
import co.instil.surge.decoders.h264.SyncH264Decoder;
import co.instil.surge.decoders.mjpeg.MjpegDecoder;
import co.instil.surge.decoders.mp4v.AsyncMp4vDecoder;
import co.instil.surge.device.DeviceExaminer;
import co.instil.surge.diagnostics.DiagnosticsTracker;
import co.instil.surge.logging.Logger;
import co.instil.surge.logging.LoggerFactory;

/**
 * Factory to generate each supported type of decoder.
 */
public final class DecoderFactory {
    private static final Logger LOGGER = LoggerFactory.getLogger(DecoderFactory.class);
    private static final DeviceExaminer DEVICE_EXAMINER = new DeviceExaminer();

    private DecoderFactory() {}

    public static Decoder generateH264Decoder(SurgeVideoView videoView, DiagnosticsTracker diagnostics) {
        if (DEVICE_EXAMINER.isPreLollipopDevice()) {
            LOGGER.info("Detecting Lollipop decoder");
            return new SyncH264Decoder(videoView, diagnostics);
        }

        return new AsyncH264Decoder(videoView, diagnostics);
    }

    public static Decoder generateMP4VDecoder(SurgeVideoView videoView, DiagnosticsTracker diagnostics) {
        if (DEVICE_EXAMINER.isPreLollipopDevice()) {
            throw new UnsupportedOperationException("No Mp4V decoder available for API 19");
        }

        return new AsyncMp4vDecoder(videoView, diagnostics);
    }

    public static Decoder generateMJPEGDecoder(SurgeVideoView videoView, DiagnosticsTracker diagnostics) {
        return new MjpegDecoder(videoView, diagnostics);
    }
}
