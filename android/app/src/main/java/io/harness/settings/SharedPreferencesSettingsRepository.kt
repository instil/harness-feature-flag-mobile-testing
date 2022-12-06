package io.harness.settings

import android.content.Context
import android.util.Log
import androidx.preference.PreferenceManager
import dagger.hilt.android.qualifiers.ApplicationContext
import javax.inject.Inject

typealias SettingChangeListener = (String)->Unit

class SharedPreferencesSettingsRepository @Inject constructor(@ApplicationContext context: Context) :
    SettingsRepository {

    private val sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context)

    private val listeners = mutableMapOf<String, MutableList<SettingChangeListener>>()

    override fun get(key: String, default: String): String =
        sharedPreferences.getString(key, default)?.trim() ?: default

    override fun set(key: String, value: String) {
        sharedPreferences
            .edit()
            .putString(key, value.trim())
            .apply()
        notifyListeners(key, value)
    }

    override fun registerListener(vararg keys: String, listener: (String) -> Unit) {
        for (key in keys) {
            if (listeners.containsKey(key)) {
                listeners[key]?.add(listener)
            } else {
                listeners[key] = mutableListOf(listener)
            }
        }
    }

    private fun notifyListeners(key: String, value: String) {
        Log.d(TAG, "Notifying listeners to change of setting: $key")
        listeners[key]?.forEach { it(value) }
    }

    companion object {
        private const val TAG = "SettingsRepository"
    }
}