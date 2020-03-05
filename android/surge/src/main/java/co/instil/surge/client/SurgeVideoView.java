/*
 * Copyright (c) 2020 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.client;

import android.graphics.Matrix;
import android.graphics.Point;
import android.util.Size;
import android.view.Surface;
import android.view.TextureView;

/**
 * Created by paulshields on 12/05/2017.
 */
public class SurgeVideoView {
    public static final SurgeVideoView NONE = new SurgeVideoView(null, SurgeVideoScale.MATCH_ASPECT_RATIO);
    private static final Matrix IDENTITY_MATRIX = new Matrix();

    private TextureView textureView;
    private SurgeVideoScale videoScale;
    private int videoWidth;
    private int videoHeight;

    public SurgeVideoView(TextureView textureView, SurgeVideoScale videoScale) {
        this.textureView = textureView;
        this.videoScale = videoScale;
        videoWidth = -1;
        videoHeight = -1;
    }

    public Surface generateUniqueSurface() {
        if (textureView == null) {
            return null;
        }

        return new Surface(textureView.getSurfaceTexture());
    }

    public TextureView getTextureView() {
        return textureView;
    }

    public int getViewWidth() {
        return textureView.getWidth();
    }

    public int getViewHeight() {
        return textureView.getHeight();
    }

    public SurgeVideoScale getVideoScale() {
        return videoScale;
    }

    public void setVideoScale(SurgeVideoScale videoScale) {
        this.videoScale = videoScale;
        refreshVideoScaling();
    }

    public void setVideoDimensions(int width, int height) {
        if (width == videoWidth && height == videoHeight) {
            return;
        }

        this.videoWidth = width;
        this.videoHeight = height;
        refreshVideoScaling();
    }

    private void refreshVideoScaling() {
        if (videoScale == SurgeVideoScale.MATCH_ASPECT_RATIO) {
            scaleVideoPlayerForVideoDimensions();
        } else {
            resetVideoPlayerTransform();
        }
    }

    private void scaleVideoPlayerForVideoDimensions() {
        double aspectRatio = (double) videoHeight / videoWidth;

        Size videoPlayerSize = generatePlayerSizeForAspectRatio(aspectRatio);
        Point videoPlayerOffsetPoint = generateScaledPlayerCenterOffsetPoint(videoPlayerSize);
        applyTransformToView(videoPlayerSize, videoPlayerOffsetPoint);
    }

    private Size generatePlayerSizeForAspectRatio(double aspectRatio) {
        if (playerIsLimitedByViewWidth(aspectRatio)) {
            return new Size(getViewWidth() , (int) (getViewWidth() * aspectRatio));
        } else {
            return new Size((int) (getViewHeight() / aspectRatio), getViewHeight());
        }
    }

    private boolean playerIsLimitedByViewWidth(double aspectRatio) {
        return getViewWidth() > (int) (getViewWidth() * aspectRatio);
    }

    private Point generateScaledPlayerCenterOffsetPoint(Size videoPlayerSize) {
        int videoPlayerXOffset = (getViewWidth() - videoPlayerSize.getWidth()) / 2;
        int videoPlayerYOffset = (getViewHeight() - videoPlayerSize.getHeight()) / 2;
        return new Point(videoPlayerXOffset, videoPlayerYOffset);
    }

    private void applyTransformToView(Size videoPlayerSize, Point offsetPoint) {
        Matrix matrix = new Matrix();
        textureView.getTransform(matrix);
        matrix.setScale((float) videoPlayerSize.getWidth() / getViewWidth() , (float) videoPlayerSize.getHeight() / getViewHeight());
        matrix.postTranslate(offsetPoint.x, offsetPoint.y);
        textureView.setTransform(matrix);
    }

    private void resetVideoPlayerTransform() {
        textureView.setTransform(IDENTITY_MATRIX);
    }
}
