# Harness Feature Flag Mobile Testing App

This repo contains mobile apps and an automated test suite designed to test the performance and resilency of the [Harness Feature Flag](https://harness.io/products/feature-flags) system.

## iOS

The iOS app is built using:

- [Harness iOS feature flag SDK](https://github.com/harness/ff-ios-client-sdk)
- [SwiftUI](https://developer.apple.com/xcode/swiftui/)
- [Swinject](https://github.com/Swinject/Swinject)
- [SwinjectAutoregistration](https://github.com/Swinject/SwinjectAutoregistration)

The external packages are integrated using [Swift Package Manager](https://www.swift.org/package-manager/).

## Building the app

The app is provided as a standard iOS project. Simply open `apple/Harness.xcodeproj` in Xcode.

You will need to provide your own Harness API key and target to run the app. First, you need to create a copy of the configuration file:

`cp apple/Harness/Configuration/ApiKeys.xcconfig_sample apple/Harness/Configuration/ApiKeys.xcconfig`

Then you must edit that file, adding in you own API key and target. You may also need to configure the feature flag keys contained in that file to match the IDs of the feature flags contained in your own Harness account.

Once that is complete, you should be able to build and run the app in the same way as any other iOS project.

If Xcode does not acquire the external dependencies automatically, you ca run this process manually by navigating to `File >> Packages >> Resolve Package Versions`.

## Running the Automated Test Suite

The automated test suite is designed to test the performance and resilency of the iOS Feature Flag SDK. It can be run from the command line using the following command

`xcodebuild test -project apple/Harness.xcodeproj -scheme HarnessUITests -destination 'platform=iOS Simulator,name=iPhone 14'`

Each test can also be run individually from Xcode. Simply navigate to any of the tests in the HarnessUITests package and click the diamond icon in the left hand line count bar.

## Producing a HTML report from Automated Test Suite

xcpretty is a tool for formatting the output of xcodebuild.

It can be installed using:

`gem install xcpretty`

The results of an xcode build can then be piped to xcpretty to create a HTML report

`xcodebuild test -project apple/Harness.xcodeproj -scheme HarnessUITests -destination 'platform=iOS Simulator,name=iPhone 14' | xcpretty --report html`

xcpretty can also be used to produce a JUnit-style XML report compatible with Jenkins and TeamCity CI.
If running xcpretty on a CI like Travis or Jenkins, you may want to exit with same status code as xcodebuild. CI systems usually use status codes to determine if the build has failed.

`xcodebuild test -project apple/Harness.xcodeproj -scheme HarnessUITests -destination 'platform=iOS Simulator,name=iPhone 14' | xcpretty --report junit && exit ${PIPESTATUS[0]}`

The output of xcpretty can be found in `build/reports/tests.html` or `build/reports/junit.xml` depending on which report was chosen.
