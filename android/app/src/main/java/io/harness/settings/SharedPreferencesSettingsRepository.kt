package io.harness.settings

import android.content.Context
import androidx.preference.PreferenceManager
import dagger.hilt.android.qualifiers.ApplicationContext
import javax.inject.Inject

class SharedPreferencesSettingsRepository @Inject constructor(@ApplicationContext context: Context) :
    SettingsRepository {

    private val sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context)

    override fun get(key: String, default: String): String =
        sharedPreferences.getString(key, default) ?: default

    override fun set(key: String, value: String) {
        sharedPreferences
            .edit()
            .putString(key, value)
            .apply()
    }
}