package io.harness.settings

interface SettingsRepository {

    fun get(key: String, default: String): String
    fun set(key: String, value: String)

}