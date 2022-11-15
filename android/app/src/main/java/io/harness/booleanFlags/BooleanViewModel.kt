package io.harness.booleanFlags

import android.util.Log
import androidx.compose.runtime.State
import androidx.compose.runtime.mutableStateOf
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
class BooleanViewModel @Inject constructor(private val featureFlagService: FeatureFlagService) : ViewModel() {

    private var receivedResponses = 0
        set(value) {
            field = value
            _isLoading.value = field < 5
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
            featureFlagService.boolVariation("Boolean1") {
                Log.d("BOOLEAN", "Boolean 1 evaluation: $it")
                _booleanOne.value = it
                receivedResponses++
            }

            featureFlagService.boolVariation("Boolean2") {
                Log.d("BOOLEAN", "Boolean 2 evaluation: $it")
                _booleanTwo.value = it
                receivedResponses++
            }

            featureFlagService.boolVariation("Boolean3") {
                Log.d("BOOLEAN", "Boolean 3 evaluation: $it")
                _booleanThree.value = it
                receivedResponses++
            }

            featureFlagService.boolVariation("Boolean4") {
                Log.d("BOOLEAN", "Boolean 4 evaluation: $it")
                _booleanFour.value = it
                receivedResponses++
            }

            featureFlagService.boolVariation("Boolean5") {
                Log.d("BOOLEAN", "Boolean 5 evaluation: $it")
                _booleanFive.value = it
                receivedResponses++
            }
        }
    }

}