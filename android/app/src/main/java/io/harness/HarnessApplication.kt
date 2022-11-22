package io.harness

import android.app.Application
import android.util.Log
import dagger.hilt.android.HiltAndroidApp
import io.harness.services.FeatureFlagService
import io.harness.settings.SettingsRepository
import kotlinx.coroutines.MainScope
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltAndroidApp
class HarnessApplication : Application() {

    @Inject
    @SwappableService
    lateinit var featureFlagService: FeatureFlagService

    @Inject
    lateinit var settingsRepository: SettingsRepository

    override fun onCreate() {
        super.onCreate()
        MainScope().launch {
            featureFlagService.load {
                Log.d("HARNESS APP", "Feature flag service has finished loading")
            }
        }
    }

    override fun onTerminate() {
        super.onTerminate()
        featureFlagService.destroy()
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