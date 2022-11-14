package io.harness.booleanFlags

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
                    Row(modifier = Modifier
                        .fillMaxWidth()
                        .padding(24.dp), horizontalArrangement = Arrangement.Center) {
                        Text(text = "Loading...", fontSize = 28.sp, fontWeight = FontWeight.Bold, modifier = Modifier.alpha(if (viewModel.isLoading.value) 1f else 0f))
                    }
                    Column(
                        modifier = Modifier.fillMaxSize(),
                        horizontalAlignment = Alignment.CenterHorizontally,
                        verticalArrangement = Arrangement.SpaceEvenly) {
                        BooleanRow(label = "Boolean One", state = viewModel.booleanOne)
                        BooleanRow(label = "Boolean Two", state = viewModel.booleanTwo)
                        BooleanRow(label = "Boolean Three", state = viewModel.booleanThree)
                        BooleanRow(label = "Boolean Four", state = viewModel.booleanFour)
                        BooleanRow(label = "Boolean Five", state = viewModel.booleanFive)
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

@Composable
fun BooleanRow(label: String, state: State<Boolean>) {
    Row(horizontalArrangement = Arrangement.Center, verticalAlignment = Alignment.CenterVertically) {
        Text(text = label, fontSize = 24.sp)
        Switch(
            checked = state.value,
            onCheckedChange = {},
            enabled = false
        )
    }
}