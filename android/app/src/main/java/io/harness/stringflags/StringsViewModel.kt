package io.harness.stringflags

import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.FeatureFlagService
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
class StringsViewModel @Inject constructor(
    private val featureFlagService: FeatureFlagService
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
            _isLoading.value = field < EXPECTED_RESPONSES
        }

    fun loadStringFeatureFlags() {
        viewModelScope.launch(Dispatchers.IO) {
            featureFlagService.stringVariation("TestString") {
                Log.d(TAG, "TextString evaluation: $it")
                _testString.value = it
                receivedResponses++
            }

            featureFlagService.stringVariation("YouTubeUrl") {
                Log.d(TAG, "YouTubeUrl evaluation: $it")
                _youtubeUrl.value = it
                receivedResponses++
            }

            featureFlagService.stringVariation("WebViewUrl") {
                Log.d(TAG, "WebViewUrl evaluation: $it")
                _webViewUrl.value = it
                receivedResponses++
            }
        }
    }

    companion object {
        const val TAG = "StringsViewModel"
        const val EXPECTED_RESPONSES = 3
    }

}