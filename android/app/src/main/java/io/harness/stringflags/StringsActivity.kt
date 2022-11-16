package io.harness.stringflags

import android.content.Intent
import android.content.IntentFilter
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Language
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.filled.Videocam
import androidx.compose.runtime.*
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.google.accompanist.web.WebView
import com.google.accompanist.web.rememberWebViewState
import dagger.hilt.android.AndroidEntryPoint
import io.harness.HarnessActivity
import io.harness.HarnessApplication.Companion.ACTION_STRING_UPDATE
import io.harness.HarnessApplication.Companion.EXTRA_EVALUATION_VALUE
import io.harness.ui.theme.HarnessTheme

@AndroidEntryPoint
class StringsActivity : HarnessActivity<String>() {

    private val viewModel: StringsViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            HarnessTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colors.background
                ) {
                    SectionTabs()
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        viewModel.loadStringFeatureFlags()
    }

    private data class TabInfo(val title: String, val icon: ImageVector)

    @Composable
    fun SectionTabs() {
        var state by remember { mutableStateOf(0) }
        val isLoadingState = viewModel.isLoading.observeAsState(true)
        val testStringState = viewModel.testString.observeAsState("")
        val youtubeUrlState = rememberWebViewState(url = "https://youtube.com/watch?v=${viewModel.youtubeUrl.value}")
        val webviewUrlState = rememberWebViewState(url = viewModel.webViewUrl.value ?: "" )

        val tabs = listOf(
            TabInfo("Test", Icons.Default.Settings),
            TabInfo("Video", Icons.Default.Videocam),
            TabInfo("Web", Icons.Default.Language)
        )
        Column {
            TabRow(selectedTabIndex = state) {
                tabs.forEachIndexed { index, tab ->
                    Tab(
                        text = { Text(tab.title) },
                        icon = { tab.icon },
                        selected = state == index,
                        onClick = { state = index }
                    )
                }
            }
            if (isLoadingState.value) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(24.dp), horizontalArrangement = Arrangement.Center
                ) {
                    Text(text = "Loading...", fontSize = 28.sp, fontWeight = FontWeight.Bold)
                }
            }
            Column(
                modifier = Modifier.fillMaxSize().alpha(if (isLoadingState.value) 0f else 1f),
                verticalArrangement = Arrangement.Center
            ) {
                when (state) {
                    0 -> {
                        Text(
                            modifier = Modifier.align(Alignment.CenterHorizontally),
                            text = testStringState.value,
                            style = MaterialTheme.typography.body1)
                    }
                    1 -> {
                        WebView(state = youtubeUrlState,
                        onCreated = { webview ->
                            webview.settings.apply {
                                javaScriptEnabled = true
                                domStorageEnabled = true
                            }
                        })
                    }
                    2 -> {
                        WebView(state = webviewUrlState)
                    }
                }
            }
        }
    }

    override fun getBroadcastFilter() = IntentFilter(ACTION_STRING_UPDATE)
    override fun extractValue(intent: Intent) = intent.getStringExtra(EXTRA_EVALUATION_VALUE) ?: ""
    override fun onEvaluationUpdate(flag: String, value: String) = viewModel.updateFlag(flag, value)
}