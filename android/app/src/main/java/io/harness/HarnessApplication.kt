package io.harness

import android.app.Application
import dagger.hilt.android.HiltAndroidApp

@HiltAndroidApp
class HarnessApplication : Application() {
    companion object {
        const val ACTION_BOOLEAN_UPDATE = "io.harness.ACTION_BOOLEAN_UPDATE"
        const val ACTION_STRING_UPDATE = "io.harness.ACTION_STRING_UPDATE"
        const val ACTION_INT_UPDATE = "io.harness.ACTION_NUMBER_UPDATE"
        const val ACTION_JSON_UPDATE = "io.harness.ACTION_JSON_UPDATE"
        const val EXTRA_EVALUATION_FLAG = "io.harness.EXTRA_EVALUATION_FLAG"
        const val EXTRA_EVALUATION_VALUE = "io.harness.EXTRA_EVALUATION_VALUE"
    }
}