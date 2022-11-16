package io.harness

import android.content.Intent
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.constraintlayout.compose.ConstraintLayout
import io.harness.booleanflags.BooleanActivity
import io.harness.jsonflags.JsonActivity
import io.harness.numberflags.NumbersActivity
import io.harness.settings.SettingsActivity
import io.harness.stringflags.StringsActivity
import io.harness.ui.components.LinkButton
import io.harness.ui.components.NavigationHandler
import io.harness.ui.theme.HarnessTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            HarnessTheme {
                MainLayout()
            }
        }
    }
}

@Composable
fun MainLayout () {
    Surface(
        modifier = Modifier.fillMaxSize(),
        color = MaterialTheme.colors.background
    ) {
        val context = LocalContext.current
        MainScreen(
            navigateToBooleans = { context.startActivity(Intent(context, BooleanActivity::class.java)) },
            navigateToMultivariateStrings = { context.startActivity(Intent(context, StringsActivity::class.java)) },
            navigateToMultivariateIntegers = { context.startActivity(Intent(context, NumbersActivity::class.java)) },
            navigateToJSON = { context.startActivity(Intent(context, JsonActivity::class.java)) })
    }
}

@Composable
fun MainScreen(
    navigateToBooleans: NavigationHandler,
    navigateToMultivariateStrings: NavigationHandler,
    navigateToMultivariateIntegers: NavigationHandler,
    navigateToJSON: NavigationHandler) {

    val usingHarnessSdk = remember { mutableStateOf(true) }

    ConstraintLayout(modifier = Modifier.fillMaxSize()) {
        val (topButtons, menuButtons, bottomButtons) = createRefs()
        val context = LocalContext.current

        Row(modifier = Modifier.constrainAs(topButtons) {
                top.linkTo(parent.top, margin = 24.dp)
                start.linkTo(parent.start)
                end.linkTo(parent.end)},
            verticalAlignment = Alignment.CenterVertically) {
            Text("Using Harness SDK")
            Switch(
                checked = usingHarnessSdk.value,
                onCheckedChange = {usingHarnessSdk.value = it})
            IconButton(
                onClick = { context.startActivity(Intent(context, SettingsActivity::class.java)) }) {
                Icon(Icons.Default.Settings, "Settings")
            }
        }

        Column(horizontalAlignment = Alignment.CenterHorizontally,
            modifier = Modifier.constrainAs(menuButtons) {
            top.linkTo(topButtons.bottom)
            start.linkTo(parent.start)
            end.linkTo(parent.end)
            bottom.linkTo(bottomButtons.top)
        }, verticalArrangement = Arrangement.SpaceEvenly) {
            LinkButton(
                label = "Booleans",
                icon = Icons.Default.Rule,
                onClick = navigateToBooleans)
            LinkButton(
                label = "Multi-variate Strings",
                icon = Icons.Default.FormatQuote,
                onClick = navigateToMultivariateStrings)
            LinkButton(
                label = "Multi-variate Integers",
                icon = Icons.Default.Numbers,
                onClick = navigateToMultivariateIntegers)
            LinkButton(
                label = "Multi-variate JSON",
                icon = Icons.Default.Description,
                onClick = navigateToJSON)
        }

        Column(modifier = Modifier.constrainAs(bottomButtons) {
            bottom.linkTo(parent.bottom, margin = 24.dp)
            start.linkTo(parent.start)
            end.linkTo(parent.end)
        }) {
            LinkButton(
                label = "Suspend App",
                icon = Icons.Default.Bedtime,
                fontSize = 18.sp) {}
        }
    }
}

@Preview(showBackground = true, showSystemUi = true)
@Composable
fun DefaultPreview() {
    HarnessTheme(darkTheme = true) {
        Surface(
            modifier = Modifier.fillMaxSize(),
            color = MaterialTheme.colors.background
        ) {
            MainLayout()
        }
    }
}