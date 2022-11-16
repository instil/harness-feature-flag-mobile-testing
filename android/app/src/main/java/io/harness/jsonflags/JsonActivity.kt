package io.harness.jsonflags

import android.content.Intent
import android.content.IntentFilter
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.material.Text
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.sp
import dagger.hilt.android.AndroidEntryPoint
import io.harness.HarnessActivity
import io.harness.HarnessApplication.Companion.ACTION_JSON_UPDATE
import io.harness.HarnessApplication.Companion.EXTRA_EVALUATION_VALUE
import io.harness.ui.theme.HarnessTheme

@AndroidEntryPoint
class JsonActivity : HarnessActivity<String>() {

    private val viewModel: JsonViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            HarnessTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colors.background
                ) {
                    val isLoading = viewModel.isLoading.observeAsState(true)
                    val jsonState = viewModel.json.observeAsState("")

                    Column(
                        modifier = Modifier.fillMaxSize(),
                        verticalArrangement = Arrangement.Center
                    ) {
                            if (isLoading.value) {
                                Text(
                                    modifier = Modifier.align(Alignment.CenterHorizontally),
                                    text = "Loading...",
                                    fontSize = 28.sp,
                                    fontWeight = FontWeight.Bold)
                            } else {
                                Text(
                                    modifier = Modifier.align(Alignment.CenterHorizontally),
                                    text = "${jsonState.value}"
                                )
                            }
                    }
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        viewModel.loadJsonFeatureFlags()
    }

    override fun getBroadcastFilter() = IntentFilter(ACTION_JSON_UPDATE)
    override fun extractValue(intent: Intent) = intent.getStringExtra(EXTRA_EVALUATION_VALUE) ?: ""
    override fun onEvaluationUpdate(flag: String, value: String) = viewModel.updateFlag(flag, value)
}
