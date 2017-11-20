/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.player;

/**
 *
 */
public interface SurgeRtspPlayerDelegate {
    void rtspPlayerDidTimeout();

    void rtspPlayerDidUpdateFps(int fps);
}
