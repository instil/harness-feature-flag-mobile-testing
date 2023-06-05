package io.harness.booleanflags

import android.content.Intent
import android.content.IntentFilter
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.material.Text
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Flag
import androidx.compose.material.icons.filled.Public
import androidx.compose.material.icons.filled.Science
import androidx.compose.runtime.Composable
import androidx.compose.runtime.State
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import dagger.hilt.android.AndroidEntryPoint
import io.harness.HarnessActivity
import io.harness.HarnessApplication.Companion.ACTION_BOOLEAN_UPDATE
import io.harness.HarnessApplication.Companion.EXTRA_EVALUATION_VALUE
import io.harness.R
import io.harness.ui.components.SectionTabs
import io.harness.ui.components.TabConfig
import io.harness.ui.theme.HarnessTheme

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
                    val loadingState = viewModel.isLoading.observeAsState(true)
                    val bState1 = viewModel.booleanOne.observeAsState(false)
                    val bState2 = viewModel.booleanTwo.observeAsState(false)
                    val bState3 = viewModel.booleanThree.observeAsState(false)
                    val bState4 = viewModel.booleanFour.observeAsState(false)
                    val bState5 = viewModel.booleanFive.observeAsState(false)
                    val rwState = viewModel.realWorld.observeAsState(false)
                    val fState = viewModel.flagged.observeAsState(false)

                    SectionTabs(
                        loadingState = loadingState,
                        tabs = listOf(
                            TabConfig("Test", Icons.Default.Science) { TesterView(bState1, bState2, bState3, bState4, bState5) },
                            TabConfig("Real world", Icons.Default.Public) { RealWorldView(rwState) },
                            TabConfig("Flagged", Icons.Default.Flag) { FlaggedView(fState) }
                        )
                    )
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

@Composable
fun TesterView(vararg states: State<Boolean>) {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.SpaceEvenly
    ) {
        Text(
            text = "Boolean One",
            fontSize = 24.sp,
            modifier = Modifier.alpha(if (states[0].value) 1f else 0f)
        )
        Text(
            text = "Boolean Two",
            fontSize = 24.sp,
            modifier = Modifier.alpha(if (states[1].value) 1f else 0f)
        )
        Text(
            text = "Boolean Three",
            fontSize = 24.sp,
            modifier = Modifier.alpha(if (states[2].value) 1f else 0f)
        )
        Text(
            text = "Boolean Four",
            fontSize = 24.sp,
            modifier = Modifier.alpha(if (states[3].value) 1f else 0f)
        )
        Text(
            text = "Boolean Five",
            fontSize = 24.sp,
            modifier = Modifier.alpha(if (states[4].value) 1f else 0f)
        )
    }
}

@Composable
fun RealWorldView(state: State<Boolean>) {
    Image(
        modifier = Modifier.fillMaxSize(),
        painter = painterResource(id = R.drawable.instil),
        contentDescription = "Instil office",
        alpha = if (state.value) 1f else 0f
    )
}

@Composable
fun FlaggedView(state: State<Boolean>) {
    Text(
        modifier = Modifier
            .fillMaxWidth()
            .alpha(if (state.value) 1f else 0f)
            .padding(24.dp),
        text = "If you are seeing this, then the associated feature flag is true!",
        textAlign = TextAlign.Center,
        style = MaterialTheme.typography.h4
    )
}