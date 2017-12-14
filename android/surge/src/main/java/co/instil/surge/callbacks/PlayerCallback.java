/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.callbacks;

import co.instil.surge.player.RtspErrorCode;

/**
 *
 */
public interface PlayerCallback {
    void response(RtspErrorCode errorCode);
}
