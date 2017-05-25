/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.client;

import android.graphics.SurfaceTexture;
import android.view.Surface;

/**
 * Created by paulshields on 12/05/2017.
 */
public class SurgeSurface {
    private Surface surface;
    private int width;
    private int height;

    public SurgeSurface(Surface surface, int width, int height) {
        this.surface = surface;
        this.width = width;
        this.height = height;
    }

    public SurgeSurface(SurfaceTexture surfaceTexture, int width, int height) {
        this(surfaceTexture != null ? new Surface(surfaceTexture) : null,
                width,
                height);
    }

    public Surface getSurface() {
        return surface;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }
}
