package io.harness.stringflags

import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.SwappableService
import io.harness.services.FeatureFlagService
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_STRING_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_WEBVIEW_URL_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_YOUTUBE_URL_FLAG
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
class StringsViewModel @Inject constructor(
    @SwappableService val featureFlagService: FeatureFlagService,
    private val settingsRepository: SettingsRepository
) : ViewModel() {

    private val _isLoading = MutableLiveData(false)
    val isLoading: LiveData<Boolean> = _isLoading

    private val _testString = MutableLiveData("")
    val testString: LiveData<String> = _testString

    private val _youtubeUrl = MutableLiveData("")
    val youtubeUrl: LiveData<String> = _youtubeUrl

    private val _webViewUrl = MutableLiveData("")
    val webViewUrl: LiveData<String> = _webViewUrl

    private var receivedResponses = 0
        set(value) {
            field = value
            _isLoading.postValue(field < EXPECTED_RESPONSES)
        }

    fun loadStringFeatureFlags() {
        viewModelScope.launch(Dispatchers.IO) {
            val testStringKey = settingsRepository.get(SETTING_STRING_FLAG, "test_string")
            featureFlagService.stringVariation(testStringKey) {
                Log.d(TAG, "TextString evaluation: $it")
                _testString.postValue(it)
                receivedResponses++
            }

            val youtubeVideoKey = settingsRepository.get(SETTING_YOUTUBE_URL_FLAG, "youtube_video")
            featureFlagService.stringVariation(youtubeVideoKey) {
                Log.d(TAG, "YouTubeUrl evaluation: $it")
                _youtubeUrl.postValue(it)
                receivedResponses++
            }

            val webviewUrlKey = settingsRepository.get(SETTING_WEBVIEW_URL_FLAG, "webview_url")
            featureFlagService.stringVariation(webviewUrlKey) {
                Log.d(TAG, "WebViewUrl evaluation: $it")
                _webViewUrl.postValue(it)
                receivedResponses++
            }
        }
    }

    fun updateFlag(flag: String, value: String) {
        when (flag) {
            settingsRepository.get(SETTING_STRING_FLAG, "string_flag") -> _testString.postValue(value)
            settingsRepository.get(SETTING_YOUTUBE_URL_FLAG, "youtube_url") -> _youtubeUrl.postValue(value)
            settingsRepository.get(SETTING_WEBVIEW_URL_FLAG, "webview_url") -> _webViewUrl.postValue(value)
        }
    }

    companion object {
        const val TAG = "StringsViewModel"
        const val EXPECTED_RESPONSES = 3
    }

}