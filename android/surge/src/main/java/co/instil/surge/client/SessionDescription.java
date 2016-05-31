package co.instil.surge.client;

/**
 *
 */
public class SessionDescription {

    private final SessionType type;
    private final String controlUrl;
    private final String formatParameters;
    private final String rtpMap;

    public SessionDescription(SessionType type, String controlUrl, String formatParameters, String rtpMap) {
        this.type = type;
        this.controlUrl = controlUrl;
        this.formatParameters = formatParameters;
        this.rtpMap = rtpMap;
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

    public String getRtpMap() {
        return rtpMap;
    }

    @Override
    public String toString() {
        return String.format("\n{\n\ttype: \"%s\", \n\tcontrolUrl: \"%s\", \n\tformatParameters: \"%s\", \n\trtpMap: \"%s\"\n}", type, controlUrl, formatParameters, rtpMap);
    }
}
