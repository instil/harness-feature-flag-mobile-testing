package co.instil.surge.logger;

import android.util.Log;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.GregorianCalendar;

public class Logger {
    public static final String TAG = "SurgeLib";

    public static void verbose(String message) {
        String methodIdentifier = getMethodIdentifier();
        message = setNullToBlank(message);
        Log.v(TAG, methodIdentifier + message);
    }

    public static void debug(String message) {
        String methodIdentifier = getMethodIdentifier();
        message = setNullToBlank(message);
        Log.d(TAG, methodIdentifier + message);
    }

    public static void info(String message) {
        String methodIdentifier = getMethodIdentifier();
        message = setNullToBlank(message);
        Log.i(TAG, methodIdentifier + message);
    }

    public static void warning(String message) {
        String methodIdentifier = getMethodIdentifier();
        message = setNullToBlank(message);
        Log.w(TAG, methodIdentifier + message);
    }

    public static void error(String message) {
        String methodIdentifier = getMethodIdentifier();
        message = setNullToBlank(message);
        Log.e(TAG, methodIdentifier + message);
    }

    private static String getMethodIdentifier() {
        String methodIdentifier = "";
        StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
        if (stackTrace.length >= 4) {
            StackTraceElement currentStackTraceElement = stackTrace[4];
            String[] currentClassNameSplit = currentStackTraceElement.getClassName().split("\\.");
            String currentClassName = currentClassNameSplit[currentClassNameSplit.length - 1];
            String currentMethodName = currentStackTraceElement.getMethodName();
            int currentLineNumber = currentStackTraceElement.getLineNumber();

            methodIdentifier = "[" + currentClassName + "::" + currentMethodName + ":" + currentLineNumber + "] ";
        }
        return methodIdentifier;
    }

    private static String setNullToBlank(String string) {
        return string == null ? " " : string;
    }
}
