package io.harness.stringflags

import android.content.Intent
import android.content.IntentFilter
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.wrapContentHeight
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.material.Text
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Language
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.filled.Videocam
import androidx.compose.runtime.*
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.viewinterop.AndroidView
import androidx.lifecycle.Lifecycle
import com.google.accompanist.web.WebView
import com.google.accompanist.web.WebViewState
import com.google.accompanist.web.rememberWebViewState
import com.pierfrancescosoffritti.androidyoutubeplayer.core.player.YouTubePlayer
import com.pierfrancescosoffritti.androidyoutubeplayer.core.player.listeners.AbstractYouTubePlayerListener
import com.pierfrancescosoffritti.androidyoutubeplayer.core.player.views.YouTubePlayerView
import dagger.hilt.android.AndroidEntryPoint
import io.harness.HarnessActivity
import io.harness.HarnessApplication.Companion.ACTION_STRING_UPDATE
import io.harness.HarnessApplication.Companion.EXTRA_EVALUATION_VALUE
import io.harness.ui.components.SectionTabs
import io.harness.ui.components.TabConfig
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
                    val loadingState = viewModel.isLoading.observeAsState(true)
                    val testStringState = viewModel.testString.observeAsState("")
                    val youtubeUrlState = viewModel.youtubeUrl.observeAsState("")
                    val webviewUrlState = viewModel.webViewUrl.observeAsState("")

                    SectionTabs (
                        loadingState = loadingState,
                        tabs = listOf(
                            TabConfig("Test", Icons.Default.Settings) { TestStringView(testStringState) },
                            TabConfig("Video", Icons.Default.Videocam) { VideoStringView(lifecycle, youtubeUrlState) },
                            TabConfig("Web", Icons.Default.Language) { WebStringView(webviewUrlState) }
                        )
                    )
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        viewModel.loadStringFeatureFlags()
    }

    override fun getBroadcastFilter() = IntentFilter(ACTION_STRING_UPDATE)
    override fun extractValue(intent: Intent) = intent.getStringExtra(EXTRA_EVALUATION_VALUE) ?: ""
    override fun onEvaluationUpdate(flag: String, value: String) = viewModel.updateFlag(flag, value)
}

@Composable
fun TestStringView(testStringState: State<String>) {
    Text(
        modifier = Modifier.fillMaxWidth(),
        text = testStringState.value,
        textAlign = TextAlign.Center,
        style = MaterialTheme.typography.h4
    )
}

@Composable
fun VideoStringView(lifecycle: Lifecycle, urlState: State<String>) {
    val context = LocalContext.current
    key(urlState.value) {
        val youtubePlayer = remember {
            YouTubePlayerView(context).apply {
                lifecycle.addObserver(this)
                enableAutomaticInitialization = false
                initialize(object : AbstractYouTubePlayerListener() {
                    override fun onReady(youTubePlayer: YouTubePlayer) {
                        youTubePlayer.cueVideo(urlState.value, 0f)
                    }
                })
            }
        }
        AndroidView(
            {
                youtubePlayer
            }, modifier = Modifier
                .fillMaxSize()
                .wrapContentHeight()
        )
    }
}

@Composable
fun WebStringView(state: State<String>) {
    val urlState = rememberWebViewState(url = state.value)
    WebView(
        state = urlState,
        captureBackPresses = false,
        onCreated = {webview ->
            webview.settings.apply {
                javaScriptEnabled = true
            }
        })
}