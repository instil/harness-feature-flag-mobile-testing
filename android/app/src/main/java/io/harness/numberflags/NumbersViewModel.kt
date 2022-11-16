package io.harness.numberflags

import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.services.FeatureFlagService
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_NUMBER_FLAG
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
class NumbersViewModel @Inject constructor(
    private val featureFlagService: FeatureFlagService,
    private val settingsRepository: SettingsRepository
): ViewModel() {

    private val _isLoading = MutableLiveData(true)
    val isLoading: LiveData<Boolean> = _isLoading

    private val _number = MutableLiveData(-1)
    val number: LiveData<Int> = _number

    fun loadNumberFeatureFlags() {
        _isLoading.postValue(true)
        viewModelScope.launch(Dispatchers.IO) {
            val numberKey = settingsRepository.get(SETTING_NUMBER_FLAG, "number")
            featureFlagService.numberVariation(numberKey) {
                Log.d(TAG, "Number ($numberKey) evaluation: $it")
                _number.postValue(it.toInt())
            }
            _isLoading.postValue(false)
        }
    }

    companion object {
        const val TAG = "NumbersViewModel"
    }

}