package io.harness.settings

interface SettingsRepository {

    fun get(key: String, default: String): String
    fun set(key: String, value: String)

    companion object {
        const val SDK_KEY_PREF = "HARNESS_SDK_KEY"
        const val TARGET_ID_PREF = "HARNESS_TARGET_ID"
        const val TARGET_NAME_PREF = "HARNESS_TARGET_NAME"
        const val BOOLEAN_ONE_PREF = "BOOLEAN_TESTER_KEY_ONE"
        const val BOOLEAN_TWO_PREF = "BOOLEAN_TESTER_KEY_TWO"
        const val BOOLEAN_THREE_PREF = "BOOLEAN_TESTER_KEY_THREE"
        const val BOOLEAN_FOUR_PREF = "BOOLEAN_TESTER_KEY_FOUR"
        const val BOOLEAN_FIVE_PREF = "BOOLEAN_TESTER_KEY_FIVE"
        const val TEST_STRING_PREF = "TEST_STRING"
        const val YOUTUBE_URL_PREF = "YOUTUBE_URL"
        const val WEB_VIEW_URL_PREF = "WEB_VIEW_URL"
    }

}