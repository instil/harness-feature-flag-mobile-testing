package io.harness

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.settings.SettingsRepository
import javax.inject.Inject

@HiltViewModel
class MainViewModel @Inject constructor(
    private val settingsRepository: SettingsRepository
) : ViewModel() {

    private val _useRealService = MutableLiveData(settingsRepository.get(SettingsRepository.SETTING_USE_REAL_SERVICE, "true").toBoolean())
    val useRealService: LiveData<Boolean> = _useRealService

    fun updateUseRealService(newValue: Boolean) {
        settingsRepository.set(SettingsRepository.SETTING_USE_REAL_SERVICE, newValue.toString())
        _useRealService.postValue(newValue)
    }

}