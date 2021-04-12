/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.logging;

import android.util.Log;

import co.instil.surge.BuildConfig;

import java.io.PrintWriter;
import java.io.StringWriter;

/**
 * Created by niallkelly on 14/09/2017.
 */

public class Logger {

    private String tag;

    public Logger(String tag) {
        this.tag = tag;
    }

    public void debug(String message) {
        if (canPrint()) {
            Log.d(tag, message);
        }
    }

    public void debug(String message, Object... arguments) {
        debug(String.format(message, arguments));
    }

    public void error(String message) {
        if (canPrint()) {
            Log.e(tag, message);
        }
    }

    public void error(String message, Exception exception) {
        error(String.format(message, exception));
    }

    public void warn(String message) {
        if (canPrint()) {
            Log.w(tag, message);
        }
    }

    public void warn(String message, Object... arguments) {
        warn(String.format(message, arguments));
    }

    public void info(String message) {
        if (canPrint()) {
            Log.i(tag, message);
        }
    }

    public void info(String message, Object... arguments) {
        info(String.format(message, arguments));
    }

    public void printStackTrace(Exception exception) {
        StringWriter stacktrace = new StringWriter();
        exception.printStackTrace(new PrintWriter(stacktrace));
        error(stacktrace.toString());
    }

    protected boolean canPrint() {
        return BuildConfig.DEBUG;
    }
}
