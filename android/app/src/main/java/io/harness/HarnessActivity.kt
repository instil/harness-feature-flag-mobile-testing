package io.harness

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_REFRESH_UI
import javax.inject.Inject

abstract class HarnessActivity<T>: ComponentActivity() {

    @Inject
    lateinit var settingsRepository: SettingsRepository
    private var refreshUI = false
    private val updateBroadcastReceiver = getBroadcastReceiver()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        refreshUI = settingsRepository.get(SETTING_REFRESH_UI, "false").toBoolean()
    }

    override fun onResume() {
        super.onResume()
        if (refreshUI) {
            val filter = getBroadcastFilter()
            LocalBroadcastManager.getInstance(this).registerReceiver(updateBroadcastReceiver, filter)
        }
    }

    override fun onPause() {
        super.onPause()
        if (refreshUI) {
            LocalBroadcastManager.getInstance(this).unregisterReceiver(updateBroadcastReceiver)
        }
    }

    private fun getBroadcastReceiver() = object: BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            val flag = intent.getStringExtra(HarnessApplication.EXTRA_EVALUATION_FLAG) ?: ""
            val value = extractValue(intent)
            if (flag.isNotEmpty()) {
                onEvaluationUpdate(flag, value)
            }
        }
    }

    abstract fun getBroadcastFilter(): IntentFilter
    abstract fun extractValue(intent: Intent): T
    abstract fun onEvaluationUpdate(flag: String, value: T)
}