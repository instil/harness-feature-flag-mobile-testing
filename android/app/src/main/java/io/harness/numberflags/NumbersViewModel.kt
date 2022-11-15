package io.harness.numberflags

import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.FeatureFlagService
import io.harness.booleanFlags.BooleanViewModel
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.NUMBER_PREF
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
class NumbersViewModel @Inject constructor(
    private val featureFlagService: FeatureFlagService,
    private val settingsRepository: SettingsRepository
): ViewModel() {

    private val _number = MutableLiveData(-1)
    val number: LiveData<Int> = _number

    fun loadNumberFeatureFlags() {
        viewModelScope.launch(Dispatchers.IO) {
            val numberKey = settingsRepository.get(NUMBER_PREF, "number")
            featureFlagService.numberVariation(numberKey) {
                Log.d(TAG, "Number ($numberKey) evaluation: $it")
                _number.postValue(it.toInt())
            }
        }
    }

    companion object {
        const val TAG = "NumbersViewModel"
    }

}