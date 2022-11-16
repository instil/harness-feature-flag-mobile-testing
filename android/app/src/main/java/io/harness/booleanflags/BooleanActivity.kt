package io.harness.booleanflags

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.*
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.material.Switch
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.State
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import dagger.hilt.android.AndroidEntryPoint
import io.harness.ui.theme.HarnessTheme

@AndroidEntryPoint
class BooleanActivity : ComponentActivity() {
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

                    Row(modifier = Modifier
                        .fillMaxWidth()
                        .padding(24.dp), horizontalArrangement = Arrangement.Center) {
                        Text(text = "Loading...", fontSize = 28.sp, fontWeight = FontWeight.Bold, modifier = Modifier.alpha(if (isLoadingState.value) 1f else 0f))
                    }

                    val booleanOneState = viewModel.booleanOne.observeAsState(false)
                    val booleanTwoState = viewModel.booleanTwo.observeAsState(false)
                    val booleanThreeState = viewModel.booleanThree.observeAsState(false)
                    val booleanFourState = viewModel.booleanFour.observeAsState(false)
                    val booleanFiveState = viewModel.booleanFive.observeAsState(false)

                    Column(
                        modifier = Modifier.fillMaxSize().alpha(if (isLoadingState.value) 0f else 1f),
                        horizontalAlignment = Alignment.CenterHorizontally,
                        verticalArrangement = Arrangement.SpaceEvenly) {
                        Text(text = "Boolean One", fontSize = 24.sp, modifier = Modifier.alpha(if (booleanOneState.value) 1f else 0f))
                        Text(text = "Boolean Two", fontSize = 24.sp, modifier = Modifier.alpha(if (booleanTwoState.value) 1f else 0f))
                        Text(text = "Boolean Three", fontSize = 24.sp, modifier = Modifier.alpha(if (booleanThreeState.value) 1f else 0f))
                        Text(text = "Boolean Four", fontSize = 24.sp, modifier = Modifier.alpha(if (booleanFourState.value) 1f else 0f))
                        Text(text = "Boolean Five", fontSize = 24.sp, modifier = Modifier.alpha(if (booleanFiveState.value) 1f else 0f))
                    }
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        viewModel.loadBooleanFeatureFlags()
    }
}