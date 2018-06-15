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
    public static final SurgeSurface NONE = new SurgeSurface((SurfaceTexture)null, 0, 0);

    private SurfaceTexture surfaceTexture;
    private Surface surface;
    private int width;
    private int height;

    public SurgeSurface(SurfaceTexture surfaceTexture, int width, int height) {
        this.surfaceTexture = surfaceTexture;
        this.width = width;
        this.height = height;
    }

    public Surface getSurface() {
        if (surfaceTexture != null) {
            surface = new Surface(surfaceTexture);
        }

        return surface;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }
}
