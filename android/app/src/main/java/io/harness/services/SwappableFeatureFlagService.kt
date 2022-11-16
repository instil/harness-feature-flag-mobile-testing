package io.harness.services

import io.harness.HarnessService
import io.harness.MockService
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_USE_REAL_SERVICE
import javax.inject.Inject

class SwappableFeatureFlagService @Inject constructor(
    @HarnessService private val harnessFeatureFlagService: FeatureFlagService,
    @MockService private val mockFeatureFlagService: FeatureFlagService,
    private val settingsRepository: SettingsRepository
) : FeatureFlagService {
    override fun load(callback: () -> Unit) {
        harnessFeatureFlagService.load(callback)
        mockFeatureFlagService.load {}
    }

    override fun destroy() {
        harnessFeatureFlagService.destroy()
        mockFeatureFlagService.destroy()
    }

    override fun boolVariation(evaluationId: String, callback: FeatureFlagCallback<Boolean>) =
        if (useRealService) {
            harnessFeatureFlagService.boolVariation(evaluationId, callback)
        } else {
            mockFeatureFlagService.boolVariation(evaluationId, callback)
        }

    override fun stringVariation(evaluationId: String, callback: FeatureFlagCallback<String>) =
        if (useRealService) {
            harnessFeatureFlagService.stringVariation(evaluationId, callback)
        } else {
            mockFeatureFlagService.stringVariation(evaluationId, callback)
        }

    override fun numberVariation(evaluationId: String, callback: FeatureFlagCallback<Number>) =
        if (useRealService) {
            harnessFeatureFlagService.numberVariation(evaluationId, callback)
        } else {
            mockFeatureFlagService.numberVariation(evaluationId, callback)
        }

    override fun jsonVariation(evaluationId: String, callback: FeatureFlagCallback<JSONString>) =
        if (useRealService) {
            harnessFeatureFlagService.jsonVariation(evaluationId, callback)
        } else {
            mockFeatureFlagService.jsonVariation(evaluationId, callback)
        }

    private val useRealService: Boolean
        get() = settingsRepository.get(SETTING_USE_REAL_SERVICE, "true").toBoolean()
}