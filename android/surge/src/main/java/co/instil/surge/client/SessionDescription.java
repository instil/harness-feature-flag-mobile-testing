/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.client;

import java.util.Objects;

/**
 *
 */
public class SessionDescription {

    private final SessionType type;
    private final String controlUrl;
    private final String formatParameters;
    private final String rtpMap;

    private final boolean isNative;
    private final int framerate;
    private final int fpsFraction;
    private final int width;
    private final int height;
    private final int bitrate;

    public SessionDescription(SessionType type, String controlUrl, String formatParameters, String rtpMap,
                              boolean isNative, int framerate, int fpsFraction, int width, int height, int bitrate) {
        this.type = type;
        this.controlUrl = controlUrl;
        this.formatParameters = formatParameters;
        this.rtpMap = rtpMap;

        this.isNative = isNative;
        this.framerate = framerate;
        this.fpsFraction = fpsFraction;
        this.width = width;
        this.height = height;
        this.bitrate = bitrate;
    }

    public SessionType getType() {
        return type;
    }

    public String getControlUrl() {
        return controlUrl;
    }

    public String getFormatParameters() {
        return formatParameters;
    }

    public String getDecoderConfig() {
        return formatParameters.substring(formatParameters.indexOf("config=") + 1);
    }

    public String getRtpMap() {
        return rtpMap;
    }

    public boolean isNative() {
        return isNative;
    }

    public int getFramerate() {
        return framerate;
    }

    public int getFpsFraction() {
        return fpsFraction;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getBitrate() {
        return bitrate;
    }

    public boolean equals(Object object) {
        if (!(object instanceof SessionDescription)) {
            return false;
        }

        SessionDescription sd = (SessionDescription)object;

        return this.getFormatParameters().equals(sd.getFormatParameters()) &&
                this.getControlUrl().equals(sd.getControlUrl());
    }

    public int hashCode() {
        return Objects.hashCode(this.getFormatParameters());
    }

    public boolean isHighResolution() {
        return (width >= 1080 || height >= 1080);
    }
}
