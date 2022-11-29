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
import io.harness.settings.SettingsRepository.Companion.SETTING_ENABLE_STREAMING
import io.harness.settings.SettingsRepository.Companion.SETTING_FLAGGED_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_JSON_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_NUMBER_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_REALWORLD_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_REFRESH_UI
import io.harness.settings.SettingsRepository.Companion.SETTING_SDK_KEY
import io.harness.settings.SettingsRepository.Companion.SETTING_TARGET_ID
import io.harness.settings.SettingsRepository.Companion.SETTING_STRING_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_WEBVIEW_URL_FLAG
import io.harness.settings.SettingsRepository.Companion.SETTING_YOUTUBE_URL_FLAG
import javax.inject.Inject

@HiltViewModel
class SettingsViewModel @Inject constructor(
    private val repository: SettingsRepository) : ViewModel() {

    private fun save(setting: String, value: Any) = repository.set(setting, value.toString())

    private val _refreshUI = MutableLiveData(repository.get(SETTING_REFRESH_UI, "").toBoolean())
    val refreshUI: LiveData<Boolean> = _refreshUI
    fun updateRefreshUI(newValue: Boolean) {
        _refreshUI.value = newValue
        save(SETTING_REFRESH_UI, newValue)
    }

    private val _sdkKey = MutableLiveData(repository.get(SETTING_SDK_KEY, ""))
    val sdkKey: LiveData<String> = _sdkKey
    fun updateSDKKey(newValue: String, persist: Boolean = false) {
        _sdkKey.value = newValue
        if (persist) save(SETTING_SDK_KEY, newValue)
    }

    private val _targetId = MutableLiveData(repository.get(SETTING_TARGET_ID, ""))
    val targetId: LiveData<String> = _targetId
    fun updateTargetId(newValue: String, persist: Boolean = false) {
        _targetId.value = newValue
        if (persist) save(SETTING_TARGET_ID, newValue)
    }

    private val _enableStreaming = MutableLiveData(repository.get(SETTING_ENABLE_STREAMING, "").toBoolean())
    val enableStreaming: LiveData<Boolean> = _enableStreaming
    fun updateEnableStreaming(newValue: Boolean) {
        _enableStreaming.value = newValue
        save(SETTING_ENABLE_STREAMING, newValue)
    }

    private val _booleanOne = MutableLiveData(repository.get(SETTING_BOOLEAN_ONE_FLAG, ""))
    val booleanOne: LiveData<String> = _booleanOne
    fun updateBooleanOne(newValue: String, persist: Boolean = false) {
        _booleanOne.value = newValue
        if (persist) save(SETTING_BOOLEAN_ONE_FLAG, newValue)
    }

    private val _booleanTwo = MutableLiveData(repository.get(SETTING_BOOLEAN_TWO_FLAG, ""))
    val booleanTwo: LiveData<String> = _booleanTwo
    fun updateBooleanTwo(newValue: String, persist: Boolean = false) {
        _booleanTwo.value = newValue
        if (persist) save(SETTING_BOOLEAN_TWO_FLAG, newValue)
    }

    private val _booleanThree = MutableLiveData(repository.get(SETTING_BOOLEAN_THREE_FLAG, ""))
    val booleanThree: LiveData<String> = _booleanThree
    fun updateBooleanThree(newValue: String, persist: Boolean = false) {
        _booleanThree.value = newValue
        if (persist) save(SETTING_BOOLEAN_THREE_FLAG, newValue)
    }

    private val _booleanFour = MutableLiveData(repository.get(SETTING_BOOLEAN_FOUR_FLAG, ""))
    val booleanFour: LiveData<String> = _booleanFour
    fun updateBooleanFour(newValue: String, persist: Boolean = false) {
        _booleanFour.value = newValue
        if (persist) save(SETTING_BOOLEAN_FOUR_FLAG, newValue)
    }

    private val _booleanFive = MutableLiveData(repository.get(SETTING_BOOLEAN_FIVE_FLAG, ""))
    val booleanFive: LiveData<String> = _booleanFive
    fun updateBooleanFive(newValue: String, persist: Boolean = false) {
        _booleanFive.value = newValue
        if (persist) save(SETTING_BOOLEAN_FIVE_FLAG, newValue)
    }

    private val _realWorld = MutableLiveData(repository.get(SETTING_REALWORLD_FLAG, ""))
    val realWorld: LiveData<String> = _realWorld
    fun updateRealWorld(newValue: String, persist: Boolean = false) {
        _realWorld.value = newValue
        if (persist) save(SETTING_REALWORLD_FLAG, newValue)
    }

    private val _flagged = MutableLiveData(repository.get(SETTING_FLAGGED_FLAG, ""))
    val flagged: LiveData<String> = _flagged
    fun updateFlagged(newValue: String, persist: Boolean = false) {
        _flagged.value = newValue
        if (persist) save(SETTING_FLAGGED_FLAG, newValue)
    }

    private val _testString = MutableLiveData(repository.get(SETTING_STRING_FLAG, ""))
    val testString: LiveData<String> = _testString
    fun updateTestString(newValue: String, persist: Boolean = false) {
        _testString.value = newValue
        if (persist) save(SETTING_STRING_FLAG, newValue)
    }

    private val _youtubeUrl = MutableLiveData(repository.get(SETTING_YOUTUBE_URL_FLAG, ""))
    val youtubeUrl: LiveData<String> = _youtubeUrl
    fun updateYoutubeUrl(newValue: String, persist: Boolean = false) {
        _youtubeUrl.value = newValue
        if (persist) save(SETTING_YOUTUBE_URL_FLAG, newValue)
    }

    private val _webviewUrl = MutableLiveData(repository.get(SETTING_WEBVIEW_URL_FLAG, ""))
    val webviewUrl: LiveData<String> = _webviewUrl
    fun updateWebViewUrl(newValue: String, persist: Boolean = false) {
        _webviewUrl.value = newValue
        if (persist) save(SETTING_WEBVIEW_URL_FLAG, newValue)
    }

    private val _number = MutableLiveData(repository.get(SETTING_NUMBER_FLAG, ""))
    val number: LiveData<String> = _number
    fun updateNumber(newValue: String, persist: Boolean = false) {
        _number.value = newValue
        if (persist) save(SETTING_NUMBER_FLAG, newValue)
    }

    private val _json = MutableLiveData(repository.get(SETTING_JSON_FLAG, ""))
    val json: LiveData<String> = _json
    fun updateJson(newValue: String, persist: Boolean = false) {
        _json.value = newValue
        if (persist) save(SETTING_JSON_FLAG, newValue)
    }

}