package co.instil.surge.client;

import java.util.Map;

/**
 *
 */
public class DescribeResponse extends Response {

    private final SessionDescription[] sessionDescriptions;

    public DescribeResponse(int statusCode, Map<String, String> headers, String body, SessionDescription[] sessionDescriptions) {
        super(statusCode, headers, body);
        this.sessionDescriptions = sessionDescriptions;
    }

    public SessionDescription[] getSessionDescriptions() {
        return sessionDescriptions;
    }

}
