/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.client;

import java.nio.ByteBuffer;

/**
 * Interface to define how the extended header information should be parsed with in Surge.
 */
public interface ExtendedHeader {
    void parseHeader(ByteBuffer buffer, int length);
}
