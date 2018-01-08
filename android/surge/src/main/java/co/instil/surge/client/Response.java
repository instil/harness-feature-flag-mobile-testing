/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.client;

import java.util.Map;

import co.instil.surge.player.RtspErrorCode;

/**
 *
 */
public class Response {

    private final RtspErrorCode statusCode;
    private final Map<String, String> headers;
    private final String body;

    public Response(int statusCode, Map<String, String> headers, String body) {
        this.statusCode = RtspErrorCode.valueOf(statusCode);
        this.headers = headers;
        this.body = body;
    }

    public RtspErrorCode getStatusCode() {
        return statusCode;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public String getBody() {
        return body;
    }

}
