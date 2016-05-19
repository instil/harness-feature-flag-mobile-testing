package co.instil.surge.client;

/**
 *
 */
public class SessionDescription {

    private final SessionType type;
    private final String controlUrl;
    private final String formatSpecificParameters;
    private final String rtpMap;

    public SessionDescription(SessionType type, String controlUrl, String formatSpecificParameters, String rtpMap) {
        this.type = type;
        this.controlUrl = controlUrl;
        this.formatSpecificParameters = formatSpecificParameters;
        this.rtpMap = rtpMap;
    }

    public SessionType getType() {
        return type;
    }

    public String getControlUrl() {
        return controlUrl;
    }

    public String getFormatSpecificParameters() {
        return formatSpecificParameters;
    }

    public String getRtpMap() {
        return rtpMap;
    }

}
