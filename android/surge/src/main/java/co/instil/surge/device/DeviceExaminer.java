/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
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
