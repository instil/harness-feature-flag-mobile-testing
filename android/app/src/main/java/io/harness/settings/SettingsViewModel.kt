package io.harness.settings

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
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

    companion object {
        const val SDK_KEY_PREF = "HARNESS_SDK_KEY"
        const val TARGET_ID_PREF = "HARNESS_TARGET_ID"
        const val TARGET_NAME_PREF = "HARNESS_TARGET_NAME"
        const val BOOLEAN_ONE_PREF = "BOOLEAN_TESTER_KEY_ONE"
        const val BOOLEAN_TWO_PREF = "BOOLEAN_TESTER_KEY_TWO"
        const val BOOLEAN_THREE_PREF = "BOOLEAN_TESTER_KEY_THREE"
        const val BOOLEAN_FOUR_PREF = "BOOLEAN_TESTER_KEY_FOUR"
        const val BOOLEAN_FIVE_PREF = "BOOLEAN_TESTER_KEY_FIVE"
    }

}