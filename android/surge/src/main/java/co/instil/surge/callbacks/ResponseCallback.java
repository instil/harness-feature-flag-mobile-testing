/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.callbacks;

import co.instil.surge.client.Response;

/**
 * Created by paulshields on 05/04/2017.
 */

public interface ResponseCallback {
    void response(Response response);
}
