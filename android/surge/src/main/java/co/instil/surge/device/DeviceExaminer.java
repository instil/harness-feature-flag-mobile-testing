/*
 * Copyright (c) 2016 Instil Software.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

package co.instil.surge.device;

import android.os.Build;

/**
 * Utility class used to inspect the capabilities of Android devices e.g. operating system versions,
 * hardware capabilities etc.
 */
public class DeviceExaminer {

    /*
     * Determines whether the current device is running an older version of Android than
     * Lollipop.
     * @return true if the Android version on the device is pre-Lollipop or false otherwise.
     */
    public boolean isPreLollipopDevice() {
        return !isRunningOSVersionOrGreater(Build.VERSION_CODES.LOLLIPOP);
    }

    /*
     * Determines whether the current device is running the supplied Android OS build version or
     * newer.
     * @return true if the Android version is the supplied version, or newer.
     */
    public boolean isRunningOSVersionOrGreater(int buildVersion) {
        return Build.VERSION.SDK_INT >= buildVersion;
    }

}
