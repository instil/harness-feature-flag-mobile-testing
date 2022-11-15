package io.harness.settings

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.settings.SettingsRepository.Companion.BOOLEAN_FIVE_PREF
import io.harness.settings.SettingsRepository.Companion.BOOLEAN_FOUR_PREF
import io.harness.settings.SettingsRepository.Companion.BOOLEAN_ONE_PREF
import io.harness.settings.SettingsRepository.Companion.BOOLEAN_THREE_PREF
import io.harness.settings.SettingsRepository.Companion.BOOLEAN_TWO_PREF
import io.harness.settings.SettingsRepository.Companion.SDK_KEY_PREF
import io.harness.settings.SettingsRepository.Companion.TARGET_ID_PREF
import io.harness.settings.SettingsRepository.Companion.TARGET_NAME_PREF
import io.harness.settings.SettingsRepository.Companion.TEST_STRING_PREF
import io.harness.settings.SettingsRepository.Companion.WEB_VIEW_URL_PREF
import javax.inject.Inject

@HiltViewModel
class SettingsViewModel @Inject constructor(
    private val repository: SettingsRepository) : ViewModel() {

    private val _sdkKey = MutableLiveData(repository.get(SDK_KEY_PREF, ""))
    val sdkKey: LiveData<String> = _sdkKey

    fun updateSDKKey(newValue: String) {
        repository.set(SDK_KEY_PREF, newValue)
        _sdkKey.value = newValue
    }

    private val _targetId = MutableLiveData(repository.get(TARGET_ID_PREF, ""))
    val targetId: LiveData<String> = _targetId

    fun updateTargetId(newValue: String) {
        repository.set(TARGET_ID_PREF, newValue)
        _targetId.value = newValue
    }

    private val _targetName = MutableLiveData(repository.get(TARGET_NAME_PREF, ""))
    val targetName: LiveData<String> = _targetName

    fun updateTargetName(newValue: String) {
        repository.set(TARGET_NAME_PREF, newValue)
        _targetName.value = newValue
    }

    private val _booleanOne = MutableLiveData(repository.get(BOOLEAN_ONE_PREF, ""))
    val booleanOne: LiveData<String> = _booleanOne

    fun updateBooleanOne(newValue: String) {
        repository.set(BOOLEAN_ONE_PREF, newValue)
        _booleanOne.value = newValue
    }

    private val _booleanTwo = MutableLiveData(repository.get(BOOLEAN_TWO_PREF, ""))
    val booleanTwo: LiveData<String> = _booleanTwo

    fun updateBooleanTwo(newValue: String) {
        repository.set(BOOLEAN_TWO_PREF, newValue)
        _booleanTwo.value = newValue
    }

    private val _booleanThree = MutableLiveData(repository.get(BOOLEAN_THREE_PREF, ""))
    val booleanThree: LiveData<String> = _booleanThree

    fun updateBooleanThree(newValue: String) {
        repository.set(BOOLEAN_THREE_PREF, newValue)
        _booleanThree.value = newValue
    }

    private val _booleanFour = MutableLiveData(repository.get(BOOLEAN_FOUR_PREF, ""))
    val booleanFour: LiveData<String> = _booleanFour

    fun updateBooleanFour(newValue: String) {
        repository.set(BOOLEAN_FOUR_PREF, newValue)
        _booleanFour.value = newValue
    }

    private val _booleanFive = MutableLiveData(repository.get(BOOLEAN_FIVE_PREF, ""))
    val booleanFive: LiveData<String> = _booleanFive

    fun updateBooleanFive(newValue: String) {
        repository.set(BOOLEAN_FIVE_PREF, newValue)
        _booleanFive.value = newValue
    }

    private val _testString = MutableLiveData(repository.get(TEST_STRING_PREF, ""))
    val testString: LiveData<String> = _testString

    fun updateTestString(newValue: String) {
        repository.set(TEST_STRING_PREF, newValue)
        _testString.value = newValue
    }

    private val _webviewUrl = MutableLiveData(repository.get(WEB_VIEW_URL_PREF, ""))
    val webviewUrl: LiveData<String> = _webviewUrl

    fun updateWebViewUrl(newValue: String) {
        repository.set(WEB_VIEW_URL_PREF, newValue)
        _webviewUrl.value = newValue
    }

}