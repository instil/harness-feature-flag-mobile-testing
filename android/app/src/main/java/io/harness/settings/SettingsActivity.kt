package io.harness.settings

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.State
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import dagger.hilt.android.AndroidEntryPoint
import io.harness.ui.theme.HarnessTheme

@AndroidEntryPoint
class SettingsActivity : ComponentActivity() {
    private val viewModel: SettingsViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        actionBar?.setDisplayHomeAsUpEnabled(true)
        setContent {
            HarnessTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colors.background
                ) {
                    Column(
                        verticalArrangement = Arrangement.Top,
                        modifier = Modifier
                            .padding(36.dp)
                            .verticalScroll(rememberScrollState())
                    ) {
                        GeneralSection()
                        SdkSection()
                        BooleanSection()
                        StringSection()
                        NumberSection()
                        JsonSection()
                    }
                }
            }
        }
    }

    @Composable
    private fun GeneralSection() {
        val refreshUI = viewModel.refreshUI.observeAsState(false)

        Section(title = "General") {
            Row(Modifier.fillMaxWidth(), verticalAlignment = Alignment.CenterVertically) {
                Text(text = "Refresh UI automatically?")
                Switch(
                    checked = refreshUI.value,
                    onCheckedChange = { viewModel.updateRefreshUI(it) })
            }
        }
    }

    @Composable
    private fun SdkSection() {
        val sdkKeyState = viewModel.sdkKey.observeAsState("")
        val targetIdState = viewModel.targetId.observeAsState("")

        Section(title = "Harness SDK") {
            StringSetting(
                label = "SDK Key",
                placeholder = "Provide your Harness SDK key",
                state = sdkKeyState,
                onValueChange = { viewModel.updateSDKKey(it) },
                onDone = { viewModel.updateSDKKey(it, persist = true) }
            )
            StringSetting(
                label = "Target ID",
                placeholder = "Provide a target ID.",
                state = targetIdState,
                onValueChange = { viewModel.updateTargetId(it) },
                onDone = { viewModel.updateTargetId(it, persist = true) }
            )
        }
    }

    @Composable
    private fun BooleanSection() {
        val booleanOneState = viewModel.booleanOne.observeAsState("")
        val booleanTwoState = viewModel.booleanTwo.observeAsState("")
        val booleanThreeState = viewModel.booleanThree.observeAsState("")
        val booleanFourState = viewModel.booleanFour.observeAsState("")
        val booleanFiveState = viewModel.booleanFive.observeAsState("")
        val realWorldState = viewModel.realWorld.observeAsState("")
        val flaggedState = viewModel.flagged.observeAsState("")
        Section(title = "Boolean flags") {
            StringSetting(
                label = "Boolean One",
                placeholder = "Boolean tester flag one",
                state = booleanOneState,
                onValueChange = { viewModel.updateBooleanOne(it) },
                onDone = { viewModel.updateBooleanOne(it, persist = true) }
            )
            StringSetting(
                label = "Boolean Two",
                placeholder = "Boolean tester flag two",
                state = booleanTwoState,
                onValueChange = { viewModel.updateBooleanTwo(it) },
                onDone = { viewModel.updateBooleanTwo(it, persist = true) }
            )
            StringSetting(
                label = "Boolean Three",
                placeholder = "Boolean tester flag three",
                state = booleanThreeState,
                onValueChange = { viewModel.updateBooleanThree(it) },
                onDone = { viewModel.updateBooleanFour(it, persist = true) }
            )
            StringSetting(
                label = "Boolean Four",
                placeholder = "Boolean tester flag four",
                state = booleanFourState,
                onValueChange = { viewModel.updateBooleanFour(it) },
                onDone = { viewModel.updateBooleanFour(it, persist = true) }
            )
            StringSetting(
                label = "Boolean Five",
                placeholder = "Boolean tester flag five",
                state = booleanFiveState,
                onValueChange = { viewModel.updateBooleanFive(it) },
                onDone = { viewModel.updateBooleanFive(it, persist = true) }
            )
            StringSetting(
                label = "Real World",
                placeholder = "Real world boolean flag",
                state = realWorldState,
                onValueChange = { viewModel.updateRealWorld(it) },
                onDone = { viewModel.updateRealWorld(it, persist = true) }
            )
            StringSetting(
                label = "Flagged",
                placeholder = "Flagged boolean flag",
                state = flaggedState,
                onValueChange = { viewModel.updateFlagged(it) },
                onDone = { viewModel.updateFlagged(it, persist = true) }
            )
        }
    }

    @Composable
    private fun StringSection() {
        val testStringState = viewModel.testString.observeAsState("")
        val youtubeUrlState = viewModel.youtubeUrl.observeAsState("")
        val webviewUrlState = viewModel.webviewUrl.observeAsState("")

        Section("String flags") {
            StringSetting(
                label = "Test String",
                placeholder = "Basic test string",
                state = testStringState,
                onValueChange = { viewModel.updateTestString(it) },
                onDone = { viewModel.updateTestString(it, persist = true) }
            )
            StringSetting (
                label = "Youtube Video",
                placeholder = "Id for Youtube video",
                state = youtubeUrlState,
                onValueChange = { viewModel.updateYoutubeUrl(it) },
                onDone = { viewModel.updateYoutubeUrl(it, persist = true) }
            )
            StringSetting (
                label = "Web URL",
                placeholder = "URL for web view test",
                state = webviewUrlState,
                onValueChange = { viewModel.updateWebViewUrl(it) },
                onDone = { viewModel.updateWebViewUrl(it, persist = true) }
            )
        }
    }

    @Composable
    private fun NumberSection() {
        val numberState = viewModel.number.observeAsState("")

        Section("Number flags") {
            StringSetting(
                label = "Number",
                placeholder = "Simple multivariate number flag",
                state = numberState,
                onValueChange = { viewModel.updateNumber(it) },
                onDone = { viewModel.updateNumber(it, persist = true) }
            )
        }
    }
    
    @Composable
    private fun JsonSection() {
        val jsonState = viewModel.json.observeAsState("")
        
        Section("Json flags") {
            StringSetting(
                label = "Json",
                placeholder = "Simple multivariate json flag",
                state = jsonState,
                onValueChange = { viewModel.updateJson(it) },
                onDone = { viewModel.updateJson(it, persist = true) }
            )
        }
    }

    @Composable
    private fun Section(title: String, content: @Composable (() -> Unit)) {
        Column {
            Text(text = title, fontSize = 24.sp)
            Divider(
                thickness = 1.dp,
                color = Color.Gray,
                modifier = Modifier.padding(bottom = 12.dp)
            )
            content()
        }
    }

    @Composable
    private fun StringSetting(
        label: String,
        placeholder: String,
        state: State<String>,
        onValueChange: (String) -> Unit,
        onDone: (String) -> Unit
    ) {
        Column {
            val focusRequester = LocalFocusManager.current
            TextField(
                modifier = Modifier.fillMaxWidth(),
                value = state.value,
                label = { Text(text = label, fontWeight = FontWeight.Bold) },
                placeholder = { Text(placeholder) },
                onValueChange = onValueChange,
                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Done),
                keyboardActions = KeyboardActions(onDone = {
                    focusRequester.clearFocus()
                    onDone(state.value)
                })
            )
        }
    }
}

