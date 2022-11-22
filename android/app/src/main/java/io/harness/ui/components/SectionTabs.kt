package io.harness.ui.components

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Tab
import androidx.compose.material.TabRow
import androidx.compose.material.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.style.TextAlign

data class TabConfig(val title: String, val icon: ImageVector, val content: @Composable ()->Unit)

@Composable
fun SectionTabs(tabs: List<TabConfig>, loadingState: State<Boolean> = mutableStateOf(false)) {
    var chosenState by remember { mutableStateOf(0) }
    
    Column {
        TabRow(selectedTabIndex = chosenState) {
            tabs.forEachIndexed { index, tab ->
                Tab(
                    text = { Text(tab.title) },
                    icon = { tab.icon },
                    selected = chosenState == index,
                    onClick = { chosenState = index })
            }
        }
        Column(modifier = Modifier.fillMaxSize(), verticalArrangement = Arrangement.Center) {
            if (loadingState.value) {
                Text(
                    modifier = Modifier.fillMaxWidth(),
                    text = "Loading...",
                    textAlign = TextAlign.Center,
                    style = MaterialTheme.typography.h4)
            } else {
                tabs[chosenState].content()
            }
        }
    }
}