package io.harness.jsonflags

import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.services.FeatureFlagService
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_JSON_FLAG
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
class JsonViewModel @Inject constructor(
    private val featureFlagService: FeatureFlagService,
    private val settingsRepository: SettingsRepository
): ViewModel() {

    private val _isLoading = MutableLiveData(true)
    val isLoading: LiveData<Boolean> = _isLoading

    private val _json = MutableLiveData("")
    val json: LiveData<String> = _json

    fun loadJsonFeatureFlags() {
        viewModelScope.launch(Dispatchers.IO) {
            val jsonKey = settingsRepository.get(SETTING_JSON_FLAG, "")
            _isLoading.postValue(true)
            featureFlagService.jsonVariation(jsonKey) {
                Log.d(TAG, "Json ($jsonKey) evaluation: $it")
                _json.postValue(it)
            }
            _isLoading.postValue(false)
        }
    }

    fun updateFlag(flag: String, value: String) {
        when (flag) {
            settingsRepository.get(SETTING_JSON_FLAG, "json") -> {
                val jsonValue = value.drop(1).dropLast(1).replace("\\", "")
                _json.postValue(jsonValue)
            }
        }
    }

    companion object {
        const val TAG = "JsonViewModel"
    }

}