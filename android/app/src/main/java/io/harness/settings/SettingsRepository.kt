package io.harness.settings

interface SettingsRepository {

    fun get (key: String, default: String): String
    fun set(key: String, value: String)
    fun registerListener(vararg keys: String, listener: (String) -> Unit)

    companion object {
        const val SETTING_USE_REAL_SERVICE = "USE_REAL_SERVICE"
        const val SETTING_SDK_KEY = "HARNESS_SDK_KEY"
        const val SETTING_TARGET_ID = "HARNESS_TARGET_ID"
        const val SETTING_ENABLE_STREAMING = "ENABLE_STREAMING"
        const val SETTING_BOOLEAN_ONE_FLAG = "BOOLEAN_TESTER_KEY_ONE"
        const val SETTING_BOOLEAN_TWO_FLAG = "BOOLEAN_TESTER_KEY_TWO"
        const val SETTING_BOOLEAN_THREE_FLAG = "BOOLEAN_TESTER_KEY_THREE"
        const val SETTING_BOOLEAN_FOUR_FLAG = "BOOLEAN_TESTER_KEY_FOUR"
        const val SETTING_BOOLEAN_FIVE_FLAG = "BOOLEAN_TESTER_KEY_FIVE"
        const val SETTING_REALWORLD_FLAG = "REALWORLD"
        const val SETTING_FLAGGED_FLAG = "FLAGGED"
        const val SETTING_STRING_FLAG = "TEST_STRING"
        const val SETTING_YOUTUBE_URL_FLAG = "YOUTUBE_URL"
        const val SETTING_WEBVIEW_URL_FLAG = "WEB_VIEW_URL"
        const val SETTING_NUMBER_FLAG = "NUMBER"
        const val SETTING_JSON_FLAG = "JSON"
    }

}