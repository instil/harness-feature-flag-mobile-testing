package io.harness

import android.app.Application
import android.content.Intent
import android.util.Log
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import dagger.hilt.android.HiltAndroidApp
import io.harness.cfsdk.CfClient
import io.harness.cfsdk.cloud.core.model.Evaluation
import io.harness.cfsdk.cloud.oksse.model.StatusEvent
import io.harness.services.FeatureFlagService
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_REFRESH_UI
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
                CfClient.getInstance().registerEventsListener { result ->
                    when (result.eventType) {
                        StatusEvent.EVENT_TYPE.SSE_START -> Log.d("HARNESS APP", "SSE Stream has been opened")
                        StatusEvent.EVENT_TYPE.SSE_END -> Log.d("HARNESS APP", "SSE Stream has completed")
                        StatusEvent.EVENT_TYPE.EVALUATION_CHANGE -> {
                            val evaluation = result.extractPayload<Evaluation>()
                            Log.d("HARNESS APP", "Evaluation has changed: ${evaluation.flag} is now ${evaluation.value}")
                            if (settingsRepository.get(SETTING_REFRESH_UI, "false").toBoolean()) {
                                broadcastEvaluationChange(evaluation)
                            }
                        }
                        StatusEvent.EVENT_TYPE.EVALUATION_REMOVE -> {
                            val evaluation = result.extractPayload<Evaluation>()
                            Log.d("HARNESS APP", "Evaluation has been removed: ${evaluation.flag}")
                        }
                        StatusEvent.EVENT_TYPE.EVALUATION_RELOAD -> {
                            val evaluations = result.extractPayload<List<Evaluation>>()
                            evaluations.forEach {
                                Log.d("HARNESS APP", "Evaluation has been reloaded: ${it.flag} is now ${it.value}")
                            }
                        }
                        null -> Log.d("HARNESS APP", "Event listener received null event")
                    }
                }
            }
        }
    }

    override fun onTerminate() {
        super.onTerminate()
        featureFlagService.destroy()
    }

    private fun broadcastEvaluationChange(ev: Evaluation) {
        Log.d(TAG, "Broadcasting evaluation update for ${ev.flag}")
        Intent().also { intent ->
            intent.putExtra(EXTRA_EVALUATION_FLAG, ev.flag)
            when (ev.kind) {
                "boolean" -> {
                    intent.action = ACTION_BOOLEAN_UPDATE
                    intent.putExtra(EXTRA_EVALUATION_VALUE, (ev.value as String).toBoolean())
                }
                "string" -> {
                    intent.action = ACTION_STRING_UPDATE
                    intent.putExtra(EXTRA_EVALUATION_VALUE, ev.value as String)
                }
                "int" -> {
                    intent.action = ACTION_INT_UPDATE
                    try {
                        intent.putExtra(EXTRA_EVALUATION_VALUE, (ev.value as String).toInt())
                    } catch (e: java.lang.NumberFormatException) {
                        intent.putExtra(EXTRA_EVALUATION_VALUE, -1)
                    }
                }
                "json" -> {
                    intent.action = ACTION_JSON_UPDATE
                    intent.putExtra(EXTRA_EVALUATION_VALUE, ev.value as String)
                }
            }
            LocalBroadcastManager.getInstance(this).sendBroadcast(intent)
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