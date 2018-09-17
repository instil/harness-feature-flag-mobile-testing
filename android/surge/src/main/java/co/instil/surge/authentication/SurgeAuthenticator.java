/*
 * Copyright (c) 2018 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.authentication;

import java.util.List;

/**
 * Interface used to define the available authentication custom authentication methods available within Surge.
 */
public interface SurgeAuthenticator {
    /**
     * Provides Surge with custom headers to append to each RTSP request.
     * @param username Username provided to authenticate the stream with.
     * @param password Password provided to authenticate the stream with.
     * @return  List of headers to append to each RTSP request.
     */
    List<String> generateAuthHeadersFor(String username, String password);

    /**
     * Provides a payload to be sent, encrypted (if applicable), down the pipe to authenticate the stream
     * before Surge will attempt to execute any RTSP requests.
     * @param username Username provided to authenticate the stream with.
     * @param password Password provided to authenticate the stream with.
     * @return payload to be sent down the pipe to authenticate the RTSP stream.
     */
    byte[] firstBytesOnTheWireAuthentication(String username, String password);
}
