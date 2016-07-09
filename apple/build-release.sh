#!/usr/bin/env bash

set -e

cd $(dirname $0)

# echo "Analysing source"
# xctool -workspace Surge.xcworkspace -scheme SurgeCore analyze -failOnWarnings

echo "Executing tests"
xctool -workspace Surge.xcworkspace -scheme SurgeCore clean test \
  -reporter pretty \
  -reporter junit:build/test-results/core-lib-tests.xml

echo "Building iOS framework"
xctool -workspace Surge.xcworkspace -scheme SurgeiOS -configuration Release clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/iOS/release \
  CODE_SIGN_IDENTITY="iPhone Developer: Instil Software"

echo "Building macOS framework"
xctool -workspace Surge.xcworkspace -scheme SurgeMacOS -configuration Release clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/macOS/release \
  CODE_SIGN_IDENTITY="iPhone Developer: Instil Software"

cd build/frameworks
zip -r apple-frameworks.zip .
