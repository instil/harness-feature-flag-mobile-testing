package io.harness

import android.content.Intent
import android.os.Bundle
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
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.constraintlayout.compose.ConstraintLayout
import dagger.hilt.android.AndroidEntryPoint
import io.harness.booleanflags.BooleanActivity
import io.harness.jsonflags.JsonActivity
import io.harness.numberflags.NumbersActivity
import io.harness.settings.SettingsActivity
import io.harness.stringflags.StringsActivity
import io.harness.ui.components.LinkButton
import io.harness.ui.components.NavigationHandler
import io.harness.ui.theme.HarnessTheme

@AndroidEntryPoint
class MainActivity : ComponentActivity() {
    private val viewModel: MainViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val navigation = object: Navigation {
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

        setContent {
            HarnessTheme {
                val serviceState = viewModel.useRealService.observeAsState(initial = true)
                MainScreen(navigation, serviceState) { newServiceState -> viewModel.updateUseRealService(newServiceState) }
            }
        }
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
    updateServiceState: (Boolean) -> Unit
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
                ) {}
            }
        }
    }
}