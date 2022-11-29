package io.harness.services

typealias JSONString = String
typealias FeatureFlagCallback<T> = (T)->Unit

interface FeatureFlagService {

    fun load(callback: ()->Unit)
    fun destroy(callback: ()->Unit = {})

    fun boolVariation(evaluationId: String, callback: FeatureFlagCallback<Boolean>)
    fun stringVariation(evaluationId: String, callback: FeatureFlagCallback<String>)
    fun numberVariation(evaluationId: String, callback: FeatureFlagCallback<Number>)
    fun jsonVariation(evaluationId: String, callback: FeatureFlagCallback<JSONString>)

}