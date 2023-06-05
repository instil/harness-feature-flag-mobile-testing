package io.harness.benchmark

import android.Manifest
import androidx.benchmark.macro.*
import androidx.benchmark.macro.junit4.MacrobenchmarkRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.rule.GrantPermissionRule
import androidx.test.uiautomator.UiDevice
import androidx.test.uiautomator.UiObject
import androidx.test.uiautomator.UiSelector
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class HarnessBenchmarks {
    @get:Rule
    val benchmarkRule = MacrobenchmarkRule()

    @get:Rule
    val permissionRule: GrantPermissionRule = GrantPermissionRule.grant(Manifest.permission.READ_EXTERNAL_STORAGE)

    @Test
    fun appStartup() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(StartupTimingMetric()),
        iterations = 5,
        startupMode = StartupMode.COLD
    ) {
        pressHome()
        startActivityAndWait()
    }

    @Test
    fun startBooleanViewWithRealService() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(FrameTimingMetric()),
        iterations = 5,
        setupBlock = {
            pressHome()
            startActivityAndWait()
            enableHarnessSDK(device)
        }
    ) {
        device.findObject(UiSelector().text("Booleans")).click()
        device.pressBack()
    }

    @Test
    fun startBooleanViewWithMockService() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(FrameTimingMetric()),
        iterations = 5,
        setupBlock = {
            pressHome()
            startActivityAndWait()
            disableHarnessSDK(device)
        }
    ) {
        device.findObject(UiSelector().text("Booleans")).click()
        device.pressBack()
    }

    @Test
    fun startStringViewWithRealService() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(FrameTimingMetric()),
        iterations = 5,
        setupBlock = {
            pressHome()
            startActivityAndWait()
            enableHarnessSDK(device)
        }
    ) {
        device.findObject(UiSelector().text("Multi-variate Strings")).click()
        device.pressBack()
    }

    @Test
    fun startStringViewWithMockService() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(FrameTimingMetric()),
        iterations = 5,
        setupBlock = {
            pressHome()
            startActivityAndWait()
            disableHarnessSDK(device)
        }
    ) {
        device.findObject(UiSelector().text("Multi-variate Strings")).click()
        device.pressBack()
    }

    @Test
    fun startIntegersViewWithRealService() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(FrameTimingMetric()),
        iterations = 5,
        setupBlock = {
            pressHome()
            startActivityAndWait()
            enableHarnessSDK(device)
        }
    ) {
        device.findObject(UiSelector().text("Multi-variate Integers")).click()
        device.pressBack()
    }

    @Test
    fun startIntegersViewWithMockService() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(FrameTimingMetric()),
        iterations = 5,
        setupBlock = {
            pressHome()
            startActivityAndWait()
            disableHarnessSDK(device)
        }
    ) {
        device.findObject(UiSelector().text("Multi-variate Integers")).click()
        device.pressBack()
    }

    @Test
    fun startJsonViewWithRealService() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(FrameTimingMetric()),
        iterations = 5,
        setupBlock = {
            pressHome()
            startActivityAndWait()
            enableHarnessSDK(device)
        }
    ) {
        device.findObject(UiSelector().text("Multi-variate JSON")).click()
        device.pressBack()
    }

    @Test
    fun startJsonViewWithMockService() = benchmarkRule.measureRepeated(
        packageName = "io.harness",
        metrics = listOf(FrameTimingMetric()),
        iterations = 5,
        setupBlock = {
            pressHome()
            startActivityAndWait()
            disableHarnessSDK(device)
        }
    ) {
        device.findObject(UiSelector().text("Multi-variate JSON")).click()
        device.pressBack()
    }

    private fun enableHarnessSDK(device: UiDevice) {
        val sdkSwitch = device.findObject(UiSelector().className("android.widget.Switch"))
        if (!sdkSwitch.isChecked) sdkSwitch.click()
    }

    private fun disableHarnessSDK(device: UiDevice) {
        val sdkSwitch = device.findObject(UiSelector().className("android.widget.Switch"))
        if (sdkSwitch.isChecked) sdkSwitch.click()
    }
}