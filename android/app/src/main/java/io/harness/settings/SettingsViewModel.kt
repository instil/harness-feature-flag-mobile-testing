package io.harness.settings

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_FIVE_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_FOUR_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_ONE_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_THREE_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_BOOLEAN_TWO_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_JSON_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_NUMBER_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_REFRESH_UI
import io.harness.settings.SettingsRepository.Companion.SETTING_SDK_KEY
import io.harness.settings.SettingsRepository.Companion.SETTING_TARGET_ID
import io.harness.settings.SettingsRepository.Companion.SETTING_TARGET_NAME
import io.harness.settings.SettingsRepository.Companion.SETTING_STRING_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_WEBVIEW_URL_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_YOUTUBE_URL_FLAG
import javax.inject.Inject

@HiltViewModel
class SettingsViewModel @Inject constructor(
    private val repository: SettingsRepository) : ViewModel() {

    private val _refreshUI = MutableLiveData(repository.get(SETTING_REFRESH_UI, "").toBoolean())
    val refreshUI: LiveData<Boolean> = _refreshUI

    fun updateRefreshUI(newValue: Boolean) {
        repository.set(SETTING_REFRESH_UI, newValue.toString())
        _refreshUI.value = newValue
    }


    private val _sdkKey = MutableLiveData(repository.get(SETTING_SDK_KEY, ""))
    val sdkKey: LiveData<String> = _sdkKey

    fun updateSDKKey(newValue: String) {
        repository.set(SETTING_SDK_KEY, newValue)
        _sdkKey.value = newValue
    }

    private val _targetId = MutableLiveData(repository.get(SETTING_TARGET_ID, ""))
    val targetId: LiveData<String> = _targetId

    fun updateTargetId(newValue: String) {
        repository.set(SETTING_TARGET_ID, newValue)
        _targetId.value = newValue
    }

    private val _targetName = MutableLiveData(repository.get(SETTING_TARGET_NAME, ""))
    val targetName: LiveData<String> = _targetName

    fun updateTargetName(newValue: String) {
        repository.set(SETTING_TARGET_NAME, newValue)
        _targetName.value = newValue
    }

    private val _booleanOne = MutableLiveData(repository.get(SETTING_BOOLEAN_ONE_FLAG, ""))
    val booleanOne: LiveData<String> = _booleanOne

    fun updateBooleanOne(newValue: String) {
        repository.set(SETTING_BOOLEAN_ONE_FLAG, newValue)
        _booleanOne.value = newValue
    }

    private val _booleanTwo = MutableLiveData(repository.get(SETTING_BOOLEAN_TWO_FLAG, ""))
    val booleanTwo: LiveData<String> = _booleanTwo

    fun updateBooleanTwo(newValue: String) {
        repository.set(SETTING_BOOLEAN_TWO_FLAG, newValue)
        _booleanTwo.value = newValue
    }

    private val _booleanThree = MutableLiveData(repository.get(SETTING_BOOLEAN_THREE_FLAG, ""))
    val booleanThree: LiveData<String> = _booleanThree

    fun updateBooleanThree(newValue: String) {
        repository.set(SETTING_BOOLEAN_THREE_FLAG, newValue)
        _booleanThree.value = newValue
    }

    private val _booleanFour = MutableLiveData(repository.get(SETTING_BOOLEAN_FOUR_FLAG, ""))
    val booleanFour: LiveData<String> = _booleanFour

    fun updateBooleanFour(newValue: String) {
        repository.set(SETTING_BOOLEAN_FOUR_FLAG, newValue)
        _booleanFour.value = newValue
    }

    private val _booleanFive = MutableLiveData(repository.get(SETTING_BOOLEAN_FIVE_FLAG, ""))
    val booleanFive: LiveData<String> = _booleanFive

    fun updateBooleanFive(newValue: String) {
        repository.set(SETTING_BOOLEAN_FIVE_FLAG, newValue)
        _booleanFive.value = newValue
    }

    private val _testString = MutableLiveData(repository.get(SETTING_STRING_FLAG, ""))
    val testString: LiveData<String> = _testString

    fun updateTestString(newValue: String) {
        repository.set(SETTING_STRING_FLAG, newValue)
        _testString.value = newValue
    }

    private val _youtubeUrl = MutableLiveData(repository.get(SETTING_YOUTUBE_URL_FLAG, ""))
    val youtubeUrl: LiveData<String> = _youtubeUrl

    fun updateYoutubeUrl(newValue: String) {
        repository.set(SETTING_YOUTUBE_URL_FLAG, newValue)
        _youtubeUrl.value = newValue
    }

    private val _webviewUrl = MutableLiveData(repository.get(SETTING_WEBVIEW_URL_FLAG, ""))
    val webviewUrl: LiveData<String> = _webviewUrl

    fun updateWebViewUrl(newValue: String) {
        repository.set(SETTING_WEBVIEW_URL_FLAG, newValue)
        _webviewUrl.value = newValue
    }

    private val _number = MutableLiveData(repository.get(SETTING_NUMBER_FLAG, ""))
    val number: LiveData<String> = _number

    fun updateNumber(newValue: String) {
        repository.set(SETTING_NUMBER_FLAG, newValue)
        _number.value = newValue
    }

    private val _json = MutableLiveData(repository.get(SETTING_JSON_FLAG, ""))
    val json: LiveData<String> = _json

    fun updateJson(newValue: String) {
        repository.set(SETTING_JSON_FLAG, newValue)
        _json.value = newValue
    }

}