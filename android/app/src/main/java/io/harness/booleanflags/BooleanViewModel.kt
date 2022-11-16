package io.harness.booleanflags

import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.SwappableService
import io.harness.services.FeatureFlagService
import io.harness.settings.SettingsRepository
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_FIVE_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_FOUR_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_ONE_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_THREE_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_TWO_FLAG
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
class BooleanViewModel @Inject constructor(
    @SwappableService private val featureFlagService: FeatureFlagService,
    private val settingsRepository: SettingsRepository) : ViewModel() {

    private var receivedResponses = 0
        set(value) {
            field = value
            _isLoading.postValue(field < EXPECTED_RESPONSES)
        }

    private val _isLoading = MutableLiveData(true)
    val isLoading: LiveData<Boolean> = _isLoading

    private val _booleanOne = MutableLiveData(false)
    val booleanOne: LiveData<Boolean> = _booleanOne

    private val _booleanTwo = MutableLiveData(false)
    val booleanTwo: LiveData<Boolean> = _booleanTwo

    private val _booleanThree = MutableLiveData(false)
    val booleanThree: LiveData<Boolean> = _booleanThree

    private val _booleanFour = MutableLiveData(false)
    val booleanFour: LiveData<Boolean> = _booleanFour

    private val _booleanFive = MutableLiveData(false)
    val booleanFive: LiveData<Boolean> = _booleanFive

    fun loadBooleanFeatureFlags() {
        viewModelScope.launch(Dispatchers.IO) {
            val booleanOneKey = settingsRepository.get(SETTING_BOOLEAN_ONE_FLAG, "boolean_one")
            featureFlagService.boolVariation(booleanOneKey) {
                Log.d(TAG, "Boolean 1 ($booleanOneKey) evaluation: $it")
                _booleanOne.postValue(it)
                receivedResponses++
            }

            val booleanTwoKey = settingsRepository.get(SETTING_BOOLEAN_TWO_FLAG, "boolean_two")
            featureFlagService.boolVariation(booleanTwoKey) {
                Log.d(TAG, "Boolean 2 ($booleanTwoKey) evaluation: $it")
                _booleanTwo.postValue(it)
                receivedResponses++
            }

            val booleanThreeKey = settingsRepository.get(SETTING_BOOLEAN_THREE_FLAG, "boolean_three")
            featureFlagService.boolVariation(booleanThreeKey) {
                Log.d(TAG, "Boolean 3 ($booleanThreeKey) evaluation: $it")
                _booleanThree.postValue(it)
                receivedResponses++
            }

            val booleanFourKey = settingsRepository.get(SETTING_BOOLEAN_FOUR_FLAG, "boolean_four")
            featureFlagService.boolVariation(booleanFourKey) {
                Log.d(TAG, "Boolean 4 ($booleanFourKey) evaluation: $it")
                _booleanFour.postValue(it)
                receivedResponses++
            }

            val booleanFiveKey = settingsRepository.get(SETTING_BOOLEAN_FIVE_FLAG, "boolean_five")
            featureFlagService.boolVariation(booleanFiveKey) {
                Log.d(TAG, "Boolean 5 ($booleanFiveKey) evaluation: $it")
                _booleanFive.postValue(it)
                receivedResponses++
            }
        }
    }

    fun updateFlag(flag: String, value: Boolean) {
        when (flag) {
            settingsRepository.get(SETTING_BOOLEAN_ONE_FLAG, "boolean_one") -> _booleanOne.postValue(value)
            settingsRepository.get(SETTING_BOOLEAN_TWO_FLAG, "boolean_two") -> _booleanTwo.postValue(value)
            settingsRepository.get(SETTING_BOOLEAN_THREE_FLAG, "boolean_three") -> _booleanThree.postValue(value)
            settingsRepository.get(SETTING_BOOLEAN_FOUR_FLAG, "boolean_four") -> _booleanFour.postValue(value)
            settingsRepository.get(SETTING_BOOLEAN_FIVE_FLAG, "boolean_five") -> _booleanFive.postValue(value)
        }
    }

    companion object {
        const val TAG = "BooleanViewModel"
        const val EXPECTED_RESPONSES = 5
    }

}