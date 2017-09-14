/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.logging;

/**
 * Created by niallkelly on 14/09/2017.
 */

public class LoggerFactory {

    public static Logger getLogger(Class className) {
        return getLogger(className.getName());
    }

    public static Logger getLogger(String tag) {
        return new Logger(tag);
    }
}
