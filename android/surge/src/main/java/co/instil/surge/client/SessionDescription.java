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

        return this.getFormatParameters().equals(sd.getFormatParameters());
    }

    public int hashCode() {
        return Objects.hashCode(this.getFormatParameters());
    }
}
