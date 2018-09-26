/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.callbacks;

import co.instil.surge.client.Response;

/**
 * Java callback interface to allow a user to define a callback that provides a HTTP response object as a response.
 */
public interface ResponseCallback {
    void response(Response response);
}
