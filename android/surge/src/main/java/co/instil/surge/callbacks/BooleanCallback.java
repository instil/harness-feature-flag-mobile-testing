/*
 * Copyright (c) 2018 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.callbacks;

/**
 * Java callback interface to allow a user to define a callback that provides a boolean response.
 */
public interface BooleanCallback {
    void response(boolean result);
}
