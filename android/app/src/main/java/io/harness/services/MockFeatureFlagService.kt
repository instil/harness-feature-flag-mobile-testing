package io.harness.services

import javax.inject.Inject

class MockFeatureFlagService @Inject constructor() : FeatureFlagService {
    override fun load(callback: () -> Unit) = callback()

    override fun destroy(callback: () -> Unit) = callback()

    override fun boolVariation(evaluationId: String, callback: FeatureFlagCallback<Boolean>) = callback(true)

    override fun stringVariation(evaluationId: String, callback: FeatureFlagCallback<String>) = callback("Mocked string reponse.")

    override fun numberVariation(evaluationId: String, callback: FeatureFlagCallback<Number>) = callback(1)

    override fun jsonVariation(evaluationId: String, callback: FeatureFlagCallback<JSONString>) = callback("{}")
}