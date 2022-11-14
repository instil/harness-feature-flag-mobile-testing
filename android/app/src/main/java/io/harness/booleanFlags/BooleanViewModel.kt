package io.harness.booleanFlags

import android.util.Log
import androidx.compose.runtime.State
import androidx.compose.runtime.mutableStateOf
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

    private val _isLoading = mutableStateOf(true)
    val isLoading: State<Boolean> = _isLoading

    private val _booleanOne = mutableStateOf(false)
    val booleanOne: State<Boolean> = _booleanOne

    private val _booleanTwo = mutableStateOf(false)
    val booleanTwo: State<Boolean> = _booleanTwo

    private val _booleanThree = mutableStateOf(false)
    val booleanThree: State<Boolean> = _booleanThree

    private val _booleanFour = mutableStateOf(false)
    val booleanFour: State<Boolean> = _booleanFour

    private val _booleanFive = mutableStateOf(false)
    val booleanFive: State<Boolean> = _booleanFive

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