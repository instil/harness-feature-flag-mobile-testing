package io.harness.booleanflags

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.*
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.material.Text
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.core.content.ContextCompat
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import dagger.hilt.android.AndroidEntryPoint
import io.harness.HarnessActivity
import io.harness.HarnessApplication
import io.harness.HarnessApplication.Companion.ACTION_BOOLEAN_UPDATE
import io.harness.HarnessApplication.Companion.EXTRA_EVALUATION_FLAG
import io.harness.HarnessApplication.Companion.EXTRA_EVALUATION_VALUE
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_REFRESH_UI
import io.harness.ui.theme.HarnessTheme
import javax.inject.Inject

@AndroidEntryPoint
class BooleanActivity : HarnessActivity<Boolean>() {
    private val viewModel: BooleanViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContent {
            HarnessTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colors.background
                ) {
                    val isLoadingState = viewModel.isLoading.observeAsState(false)

                    if (isLoadingState.value) {
                        Column(
                            modifier = Modifier.fillMaxSize()
                                .padding(24.dp),
                            horizontalAlignment = Alignment.CenterHorizontally,
                            verticalArrangement = Arrangement.Center
                        ) {
                            Text(
                                text = "Loading...",
                                fontSize = 28.sp,
                                fontWeight = FontWeight.Bold,
                                modifier = Modifier.alpha(if (isLoadingState.value) 1f else 0f)
                            )
                        }
                    } else {

                        val booleanOneState = viewModel.booleanOne.observeAsState(false)
                        val booleanTwoState = viewModel.booleanTwo.observeAsState(false)
                        val booleanThreeState = viewModel.booleanThree.observeAsState(false)
                        val booleanFourState = viewModel.booleanFour.observeAsState(false)
                        val booleanFiveState = viewModel.booleanFive.observeAsState(false)

                        Column(
                            modifier = Modifier.fillMaxSize()
                                .alpha(if (isLoadingState.value) 0f else 1f),
                            horizontalAlignment = Alignment.CenterHorizontally,
                            verticalArrangement = Arrangement.SpaceEvenly
                        ) {
                            Text(
                                text = "Boolean One",
                                fontSize = 24.sp,
                                modifier = Modifier.alpha(if (booleanOneState.value) 1f else 0f)
                            )
                            Text(
                                text = "Boolean Two",
                                fontSize = 24.sp,
                                modifier = Modifier.alpha(if (booleanTwoState.value) 1f else 0f)
                            )
                            Text(
                                text = "Boolean Three",
                                fontSize = 24.sp,
                                modifier = Modifier.alpha(if (booleanThreeState.value) 1f else 0f)
                            )
                            Text(
                                text = "Boolean Four",
                                fontSize = 24.sp,
                                modifier = Modifier.alpha(if (booleanFourState.value) 1f else 0f)
                            )
                            Text(
                                text = "Boolean Five",
                                fontSize = 24.sp,
                                modifier = Modifier.alpha(if (booleanFiveState.value) 1f else 0f)
                            )
                        }
                    }
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        viewModel.loadBooleanFeatureFlags()
    }

    override fun getBroadcastFilter() = IntentFilter(ACTION_BOOLEAN_UPDATE)
    override fun extractValue(intent: Intent) = intent.getBooleanExtra(EXTRA_EVALUATION_VALUE, false)
    override fun onEvaluationUpdate(flag: String, value: Boolean) = viewModel.updateFlag(flag, value)

}