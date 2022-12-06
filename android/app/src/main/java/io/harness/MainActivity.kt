package io.harness

import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.runtime.*
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.constraintlayout.compose.ConstraintLayout
import androidx.lifecycle.lifecycleScope
import dagger.hilt.android.AndroidEntryPoint
import io.harness.booleanflags.BooleanActivity
import io.harness.jsonflags.JsonActivity
import io.harness.numberflags.NumbersActivity
import io.harness.services.FeatureFlagService
import io.harness.settings.SettingsActivity
import io.harness.settings.SettingsRepository
import io.harness.stringflags.StringsActivity
import io.harness.ui.components.LinkButton
import io.harness.ui.theme.HarnessTheme
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@AndroidEntryPoint
class MainActivity : ComponentActivity() {

    @Inject
    @SwappableService
    lateinit var featureFlagService: FeatureFlagService

    @Inject
    lateinit var settingsRepository: SettingsRepository

    private val viewModel: MainViewModel by viewModels()

    private val navigation = object: Navigation {
        override fun navigateToBooleans() {
            startActivity(Intent(this@MainActivity, BooleanActivity::class.java))
        }
        override fun navigateToStrings() {
            startActivity(Intent(this@MainActivity, StringsActivity::class.java))
        }
        override fun navigateToNumbers() {
            startActivity(Intent(this@MainActivity, NumbersActivity::class.java))
        }
        override fun navigateToJson() {
            startActivity(Intent(this@MainActivity, JsonActivity::class.java))
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            HarnessTheme {
                val sdkState = viewModel.useRealService.observeAsState(initial = true)
                MainScreen(
                    navigation,
                    sdkState,
                    suspendApp = {finish()},
                    quitApp = {finishAndRemoveTask()}
                ) { newSdkState -> viewModel.updateUseRealService(newSdkState) }
            }
        }
        loadFlagService { Log.d(TAG, "Flag service has been loaded.") }
        settingsRepository.registerListener(
            SettingsRepository.SETTING_TARGET_ID,
            SettingsRepository.SETTING_ENABLE_STREAMING) { reloadFlagService()}
    }

    override fun onDestroy() {
        super.onDestroy()
        unloadFlagService { Log.d(TAG, "Flag service has been unloaded.") }
    }

    private fun loadFlagService(callback: () -> Unit) {
        lifecycleScope.launch(Dispatchers.IO) { featureFlagService.load(callback) }
    }

    private fun unloadFlagService(callback: () -> Unit) {
        lifecycleScope.launch(Dispatchers.IO) { featureFlagService.destroy(callback) }
    }

    private fun reloadFlagService() {
        Log.d(TAG, "Reloading flag service ...")
        unloadFlagService {
            Log.d(TAG, "Stopped flag service, ready to restart ...")
            loadFlagService {
                Log.d(TAG, "Flag service has been restarted.")
            }
        }
    }

    companion object {
        const val TAG = "MainActivity"
    }
}

interface Navigation {
    fun navigateToBooleans()
    fun navigateToStrings()
    fun navigateToNumbers()
    fun navigateToJson()
}

@Composable
fun MainScreen(
    navigation: Navigation,
    serviceState: State<Boolean>,
    suspendApp: ()->Unit = {},
    quitApp: ()->Unit = {},
    updateServiceState: (Boolean) -> Unit,
) {
    Surface(
        modifier = Modifier.fillMaxSize(),
        color = MaterialTheme.colors.background
    ) {
        ConstraintLayout(modifier = Modifier.fillMaxSize()) {
            val (topButtons, menuButtons, bottomButtons) = createRefs()
            val context = LocalContext.current

            Row(
                modifier = Modifier.constrainAs(topButtons) {
                    top.linkTo(parent.top, margin = 24.dp)
                    start.linkTo(parent.start)
                    end.linkTo(parent.end)
                },
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text("Using Harness SDK")
                Switch(
                    modifier = Modifier.testTag("useSdk"),
                    checked = serviceState.value,
                    onCheckedChange = { updateServiceState(it) })
                IconButton(
                    onClick = {
                        context.startActivity(
                            Intent(
                                context,
                                SettingsActivity::class.java
                            )
                        )
                    }) {
                    Icon(Icons.Default.Settings, "Settings")
                }
            }

            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                modifier = Modifier.constrainAs(menuButtons) {
                    top.linkTo(topButtons.bottom)
                    start.linkTo(parent.start)
                    end.linkTo(parent.end)
                    bottom.linkTo(bottomButtons.top)
                }, verticalArrangement = Arrangement.SpaceEvenly
            ) {
                LinkButton(
                    label = "Booleans",
                    icon = Icons.Default.Rule,
                    tag = "booleans",
                    onClick = { navigation.navigateToBooleans() }
                )
                LinkButton(
                    label = "Multi-variate Strings",
                    icon = Icons.Default.FormatQuote,
                    tag = "strings",
                    onClick = { navigation.navigateToStrings() }
                )
                LinkButton(
                    label = "Multi-variate Integers",
                    icon = Icons.Default.Numbers,
                    tag = "numbers",
                    onClick = { navigation.navigateToNumbers() }
                )
                LinkButton(
                    label = "Multi-variate JSON",
                    icon = Icons.Default.Description,
                    tag = "json",
                    onClick = { navigation.navigateToJson() }
                )
            }

            Column(modifier = Modifier.constrainAs(bottomButtons) {
                bottom.linkTo(parent.bottom, margin = 24.dp)
                start.linkTo(parent.start)
                end.linkTo(parent.end)
            }) {
                LinkButton(
                    label = "Suspend App",
                    icon = Icons.Default.Bedtime,
                    fontSize = 18.sp
                ) { suspendApp() }
                LinkButton(
                    label = "Close App",
                    icon = Icons.Default.Warning,
                    fontSize = 18.sp,
                    color = Color.Red
                ) { quitApp() }
            }
        }
    }
}