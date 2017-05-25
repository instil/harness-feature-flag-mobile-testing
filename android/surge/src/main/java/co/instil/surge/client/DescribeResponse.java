/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

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
