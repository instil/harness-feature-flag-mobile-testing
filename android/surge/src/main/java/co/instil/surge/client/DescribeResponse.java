package co.instil.surge.client;

import java.util.List;

/**
 *
 */
public class DescribeResponse extends Response {

    private final SessionDescription[] sessionDescriptions;

    public DescribeResponse(SessionDescription[] sessionDescriptions) {
        this.sessionDescriptions = sessionDescriptions;
    }

    public SessionDescription[] getSessionDescriptions() {
        return sessionDescriptions;
    }

}
