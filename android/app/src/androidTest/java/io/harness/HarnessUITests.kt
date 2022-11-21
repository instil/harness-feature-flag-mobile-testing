package io.harness

import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.compose.ui.test.onNodeWithTag
import androidx.compose.ui.test.performClick
import androidx.test.ext.junit.runners.AndroidJUnit4
import io.harness.ui.theme.HarnessTheme
import io.mockk.every
import io.mockk.mockk
import io.mockk.verify
import org.junit.Assert.*
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class HarnessUITests {

    @get:Rule
    val composeTestRule = createComposeRule()

    private val mockNavigation = mockk<Navigation>()
    private val sdkState = mutableStateOf(true)

    @Before
    fun setup() {
        every { mockNavigation.navigateToBooleans() } returns Unit
        every { mockNavigation.navigateToStrings() } returns Unit
        every { mockNavigation.navigateToNumbers() } returns Unit
        every { mockNavigation.navigateToJson() } returns Unit

        composeTestRule.setContent {
            HarnessTheme {
                MainScreen(mockNavigation, sdkState) { sdkState.value = it }
            }
        }
    }

    @Test
    fun testMainScreenNavigationLinksExist() {
        composeTestRule.onNodeWithTag("booleans").assertExists()
        composeTestRule.onNodeWithTag("strings").assertExists()
        composeTestRule.onNodeWithTag("numbers").assertExists()
        composeTestRule.onNodeWithTag("json").assertExists()
    }

    @Test
    fun testRealHarnessSdkToggle() {
        composeTestRule.onNodeWithTag("useSdk").performClick()
        assertEquals(false, sdkState.value)
        composeTestRule.onNodeWithTag("useSdk").performClick()
        assertEquals(true, sdkState.value)
    }

    @Test
    fun testBooleanNavigationLinkNavigatesCorrectly() {
        composeTestRule.onNodeWithTag("booleans").performClick()
        verify { mockNavigation.navigateToBooleans() }
    }

    @Test
    fun testStringsNavigationLinkNavigatesCorrectly() {
        composeTestRule.onNodeWithTag("strings").performClick()
        verify { mockNavigation.navigateToStrings() }
    }

    @Test
    fun testNumbersNavigationLinkNavigatesCorrectly() {
        composeTestRule.onNodeWithTag("numbers").performClick()
        verify { mockNavigation.navigateToNumbers() }
    }

    @Test
    fun testJsonNavigationLinkNavigatesCorrectly() {
        composeTestRule.onNodeWithTag("json").performClick()
        verify { mockNavigation.navigateToJson() }
    }
}