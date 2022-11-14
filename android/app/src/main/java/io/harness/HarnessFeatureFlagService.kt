package io.harness

import android.content.Context
import android.util.Log
import dagger.hilt.android.qualifiers.ApplicationContext
import io.harness.cfsdk.CfClient
import io.harness.cfsdk.CfConfiguration
import io.harness.settings.SettingsRepository
import io.harness.cfsdk.cloud.model.Target
import org.json.JSONObject
import javax.inject.Inject

class HarnessFeatureFlagService @Inject constructor(
    @ApplicationContext private val context: Context,
    private val settingsRepository: SettingsRepository) : FeatureFlagService {

    override fun load(callback: () -> Unit) {
        val sdkKey = settingsRepository.get("HARNESS_SDK_KEY", "")
        val targetId = settingsRepository.get("HARNESS_TARGET_ID", "")
        val targetName = settingsRepository.get("HARNESS_TARGET_NAME", "")

        Log.d("FLAG SERVICE", "targetId: $targetId, targetName: $targetName")

        val config = CfConfiguration.builder()
            .enableStream(true)
            .build()

        val target = Target().identifier(targetId).name(targetName)

        CfClient.getInstance().initialize(
            context,
            sdkKey,
            config,
            target
        )

        callback()
    }

    override fun destroy() {
        CfClient.getInstance().destroy()
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
}