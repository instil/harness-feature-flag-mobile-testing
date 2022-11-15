package io.harness

import android.content.Intent
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.constraintlayout.compose.ConstraintLayout
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import io.harness.booleanFlags.BooleanActivity
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
        HarnessNavHost()
    }
}

@Composable
fun HarnessNavHost(
    modifier: Modifier = Modifier,
    navController: NavHostController = rememberNavController(),
    startDestination: String = "mainScreen"
){
    NavHost(
        modifier = modifier,
        navController = navController,
        startDestination = startDestination
    ) {
        composable("mainScreen") {
            val context = LocalContext.current
            MainScreen(
                navigateToBooleans = { context.startActivity(Intent(context, BooleanActivity::class.java)) },
                navigateToMultivariateStrings = { context.startActivity(Intent(context, StringsActivity::class.java)) },
                navigateToMultivariateIntegers = { context.startActivity(Intent(context, NumbersActivity::class.java)) },
                navigateToJSON = { navController.navigate("multivariateJSON") }
            )
        }
        composable("multivariateJSON") {
            MultivariateJSON()
        }
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

        TopButtons(state=usingHarnessSdk, modifier = Modifier.constrainAs(topButtons) {
            top.linkTo(parent.top, margin = 24.dp)
            start.linkTo(parent.start)
            end.linkTo(parent.end)})

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

@Composable
fun TopButtons(state: MutableState<Boolean>, modifier: Modifier) {
    val context = LocalContext.current

    Row(modifier = modifier, verticalAlignment = Alignment.CenterVertically) {
        Text("Using Harness SDK")
        Switch(checked = state.value, onCheckedChange = {state.value = it})
        IconButton(
            onClick = { context.startActivity(Intent(context, SettingsActivity::class.java)) }) {
            Icon(Icons.Default.Settings, "Settings")
        }
    }
}


@Composable
fun Booleans() {

}

@Composable
fun MultivariateStrings() {
    Text("Multivariate Strings")
}

@Composable
fun MultivariateIntegers() {
    Text("Multivariate Integers")
}

@Composable
fun MultivariateJSON() {
    Text("Multivariate JSON")
}

@Preview(showBackground = true, showSystemUi = true)
@Composable
fun DefaultPreview() {
    HarnessTheme(darkTheme = true) {
        Surface(
            modifier = Modifier.fillMaxSize(),
            color = MaterialTheme.colors.background
        ) {
            HarnessNavHost()
        }
    }
}