package io.harness.services

import android.content.Context
import android.content.Intent
import android.util.Log
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import dagger.hilt.android.qualifiers.ApplicationContext
import io.harness.HarnessApplication
import io.harness.cfsdk.CfClient
import io.harness.cfsdk.CfConfiguration
import io.harness.cfsdk.cloud.core.model.Evaluation
import io.harness.cfsdk.cloud.model.Target
import io.harness.cfsdk.cloud.oksse.EventsListener
import io.harness.cfsdk.cloud.oksse.model.StatusEvent
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_ENABLE_STREAMING
import io.harness.settings.SettingsRepository.Companion.SETTING_SDK_KEY
import io.harness.settings.SettingsRepository.Companion.SETTING_TARGET_ID
import org.json.JSONObject
import javax.inject.Inject

class HarnessFeatureFlagService @Inject constructor(
    @ApplicationContext private val context: Context,
    private val settingsRepository: SettingsRepository) : FeatureFlagService {

    override fun load(callback: () -> Unit) {
        val sdkKey = settingsRepository.get(SETTING_SDK_KEY, "")
        val targetId = settingsRepository.get(SETTING_TARGET_ID, "")
        val enableStreaming = settingsRepository.get(SETTING_ENABLE_STREAMING, "").toBoolean()

        val config = CfConfiguration.builder()
            .enableStream(enableStreaming)
            .build()

        val target = Target().identifier(targetId)

        CfClient.getInstance().initialize(context, sdkKey, config, target)

        registerEventListener()

        callback()
    }

    override fun destroy(callback: () -> Unit) {
        unregisterEventListener()
        CfClient.getInstance().destroy()

        callback()
    }

    override fun boolVariation(evaluationId: String, callback: FeatureFlagCallback<Boolean>) {
        val evaluation = CfClient.getInstance().boolVariation(evaluationId, false)
        callback(evaluation)
    }

    override fun stringVariation(evaluationId: String, callback: FeatureFlagCallback<String>) {
        val evaluation = CfClient.getInstance().stringVariation(evaluationId, "")
        callback(evaluation)
    }

    override fun numberVariation(evaluationId: String, callback: FeatureFlagCallback<Number>) {
        val evaluation = CfClient.getInstance().numberVariation(evaluationId, 0.0)
        callback(evaluation)
    }

    override fun jsonVariation(evaluationId: String, callback: FeatureFlagCallback<JSONString>) {
        val evaluation = CfClient.getInstance().jsonVariation(evaluationId, JSONObject("{}"))
        callback(evaluation.toString())
    }

    private val eventsListener = EventsListener { statusEvent ->
        when (statusEvent.eventType) {
            StatusEvent.EVENT_TYPE.SSE_START -> Log.d("HARNESS APP", "SSE Stream has been opened")
            StatusEvent.EVENT_TYPE.SSE_END -> Log.d("HARNESS APP", "SSE Stream has completed")
            StatusEvent.EVENT_TYPE.EVALUATION_CHANGE -> {
                val evaluation = statusEvent.extractPayload<Evaluation>()
                Log.d("HARNESS APP", "Evaluation has changed: ${evaluation.flag} is now ${evaluation.value}")
                if (settingsRepository.get(SETTING_ENABLE_STREAMING, "").toBoolean()) {
                    broadcastEvaluationChange(evaluation)
                }
            }
            StatusEvent.EVENT_TYPE.EVALUATION_REMOVE -> {
                val evaluation = statusEvent.extractPayload<Evaluation>()
                Log.d("HARNESS APP", "Evaluation has been removed: ${evaluation.flag}")
            }
            StatusEvent.EVENT_TYPE.EVALUATION_RELOAD -> {
                val evaluations = statusEvent.extractPayload<List<Evaluation>>()
                evaluations.forEach {
                    Log.d("HARNESS APP", "Evaluation has been reloaded: ${it.flag} is now ${it.value}")
                }
            }
            null -> Log.d("HARNESS APP", "Event listener received null event")
        }
    }

    private fun registerEventListener() {
        CfClient.getInstance().registerEventsListener(eventsListener)
    }

    private fun unregisterEventListener() {
        CfClient.getInstance().unregisterEventsListener(eventsListener)
    }

    private fun broadcastEvaluationChange(ev: Evaluation) {
        Log.d(HarnessApplication.TAG, "Broadcasting evaluation update for ${ev.flag}")
        Intent().also { intent ->
            intent.putExtra(HarnessApplication.EXTRA_EVALUATION_FLAG, ev.flag)
            when (ev.kind) {
                "boolean" -> {
                    intent.action = HarnessApplication.ACTION_BOOLEAN_UPDATE
                    intent.putExtra(HarnessApplication.EXTRA_EVALUATION_VALUE, (ev.value as String).toBoolean())
                }
                "string" -> {
                    intent.action = HarnessApplication.ACTION_STRING_UPDATE
                    intent.putExtra(HarnessApplication.EXTRA_EVALUATION_VALUE, ev.value as String)
                }
                "int" -> {
                    intent.action = HarnessApplication.ACTION_INT_UPDATE
                    try {
                        intent.putExtra(HarnessApplication.EXTRA_EVALUATION_VALUE, (ev.value as String).toInt())
                    } catch (e: java.lang.NumberFormatException) {
                        intent.putExtra(HarnessApplication.EXTRA_EVALUATION_VALUE, -1)
                    }
                }
                "json" -> {
                    intent.action = HarnessApplication.ACTION_JSON_UPDATE
                    intent.putExtra(HarnessApplication.EXTRA_EVALUATION_VALUE, ev.value as String)
                }
            }
            LocalBroadcastManager.getInstance(context).sendBroadcast(intent)
        }
    }
}