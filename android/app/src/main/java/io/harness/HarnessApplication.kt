package io.harness

import android.app.Application
import android.util.Log
import dagger.hilt.android.HiltAndroidApp
import io.harness.cfsdk.CfClient
import io.harness.cfsdk.cloud.core.model.Evaluation
import io.harness.cfsdk.cloud.oksse.model.StatusEvent
import io.harness.services.FeatureFlagService
import kotlinx.coroutines.MainScope
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltAndroidApp
class HarnessApplication : Application() {

    @Inject
    lateinit var featureFlagService: FeatureFlagService

    override fun onCreate() {
        super.onCreate()
        MainScope().launch {
            featureFlagService.load {
                Log.d("HARNESS APP", "Feature flag service has finished loading")
                CfClient.getInstance().registerEventsListener { result ->
                    when (result.eventType) {
                        StatusEvent.EVENT_TYPE.SSE_START -> Log.d("HARNESS APP", "SSE Stream has been opened")
                        StatusEvent.EVENT_TYPE.SSE_END -> Log.d("HARNESS APP", "SSE Stream has completed")
                        StatusEvent.EVENT_TYPE.EVALUATION_CHANGE -> {
                            val evaluation = result.extractPayload<Evaluation>()
                            Log.d("HARNESS APP", "Evaluation has changed: ${evaluation.flag} is now ${evaluation.value}")
                        }
                        StatusEvent.EVENT_TYPE.EVALUATION_REMOVE -> {
                            val evaluation = result.extractPayload<Evaluation>()
                            Log.d("HARNESS APP", "Evaluation has been removed: ${evaluation.flag}")
                        }
                        StatusEvent.EVENT_TYPE.EVALUATION_RELOAD -> {
                            val evaluations = result.extractPayload<List<Evaluation>>()
                            evaluations.forEach {
                                Log.d("HARNESS APP", "Evaluation has been reloaded: ${it.flag} is now ${it.value}")
                            }
                        }
                        null -> Log.d("HARNESS APP", "Event listener received null event")
                    }
                }
            }
        }
    }

    override fun onTerminate() {
        super.onTerminate()
        featureFlagService.destroy()
    }

}