package io.harness

import android.app.Application
import android.util.Log
import dagger.hilt.android.HiltAndroidApp
import io.harness.services.FeatureFlagService
import io.harness.settings.SettingChangeListener
import io.harness.settings.SettingsRepository
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltAndroidApp
class HarnessApplication : Application() {

    @Inject
    @SwappableService
    lateinit var featureFlagService: FeatureFlagService

    @Inject
    lateinit var settingsRepository: SettingsRepository

    private val featureFlagServiceScope = CoroutineScope(Dispatchers.IO)

    override fun onCreate() {
        super.onCreate()
        settingsRepository.registerListener(
            SettingsRepository.SETTING_TARGET_ID,
            SettingsRepository.SETTING_TARGET_NAME,
            listener = object: SettingChangeListener {
                override fun onSettingChange(newValue: String) {
                    Log.d(TAG, "The target id or name has changed. Reloading the feature flag service.")
                    reloadFlagService()
                }
            })
        loadFlagService {
            Log.d(TAG, "Flag service has been loaded")
        }
    }

    override fun onTerminate() {
        super.onTerminate()
        unloadFlagService {
            Log.d(TAG, "Flag service has been destroyed")
        }
    }

    private fun loadFlagService(callback: ()->Unit) {
        featureFlagServiceScope.launch {
            featureFlagService.load(callback)
        }
    }

    private fun unloadFlagService(callback: ()->Unit) {
        featureFlagServiceScope.launch {
            featureFlagService.destroy(callback)
        }
    }

    private fun reloadFlagService() {
        Log.d(TAG, "Reloading flag service...")
        unloadFlagService {
            Log.d(TAG, "Stopped flag service to reload")
            loadFlagService {
                Log.d(TAG, "Flag service has been reloaded")
            }
        }
    }


    companion object {
        const val TAG = "HarnessApplication"
        const val ACTION_BOOLEAN_UPDATE = "io.harness.ACTION_BOOLEAN_UPDATE"
        const val ACTION_STRING_UPDATE = "io.harness.ACTION_STRING_UPDATE"
        const val ACTION_INT_UPDATE = "io.harness.ACTION_NUMBER_UPDATE"
        const val ACTION_JSON_UPDATE = "io.harness.ACTION_JSON_UPDATE"
        const val EXTRA_EVALUATION_FLAG = "io.harness.EXTRA_EVALUATION_FLAG"
        const val EXTRA_EVALUATION_VALUE = "io.harness.EXTRA_EVALUATION_VALUE"
    }

}