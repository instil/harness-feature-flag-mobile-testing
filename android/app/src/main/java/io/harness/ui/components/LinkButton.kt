package io.harness.ui.components

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material.Icon
import androidx.compose.material.Text
import androidx.compose.material.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

typealias NavigationHandler = () -> Unit

@Composable
fun LinkButton(label: String, icon: ImageVector, fontSize: TextUnit = 24.sp, onClick: NavigationHandler) {
    TextButton(onClick = onClick, Modifier.padding(24.dp)) {
        Icon(icon, contentDescription = label)
        Spacer(Modifier.width(12.dp))
        Text(text = label, fontSize = fontSize)
    }
}