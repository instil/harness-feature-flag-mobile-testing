#!/usr/bin/env bash

set -e

cd $(dirname $0)

# echo "Analysing source"
# xctool -workspace Surge.xcworkspace -scheme SurgeCore analyze -failOnWarnings

echo "Executing tests"
xctool -workspace Surge.xcworkspace -scheme SurgeCore clean test \
  -reporter pretty \
  -reporter junit:build/test-results/core-lib-tests.xml

# cd SurgeCore
# slather coverage
# cd ..

xctool -workspace Surge.xcworkspace -scheme SurgeiOS clean test \
  -reporter pretty \
  -reporter junit:build/test-results/framework-tests.xml

# cd SurgeFrameworks
# slather coverage
# cd ..

echo "Building macOS framework"
xctool -workspace Surge.xcworkspace -scheme SurgeMacOS -configuration Release clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/macOS \
  CODE_SIGN_IDENTITY="iPhone Developer: Instil Software"

echo "Building iOS framework"
xctool -workspace Surge.xcworkspace -scheme SurgeiOS -configuration Release -sdk iphoneos clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/iOS \
  CODE_SIGN_IDENTITY="iPhone Developer: Instil Software"

xctool -workspace Surge.xcworkspace -scheme SurgeiOS -configuration Release -sdk iphonesimulator clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/iOS/simulator \
  CODE_SIGN_IDENTITY="iPhone Developer: Instil Software"

cd build/frameworks/iOS

if [ -d "simulator/SurgeiOS.framework/Modules/SurgeiOS.swiftmodule" ]; then
  cp -r simulator/SurgeiOS.framework/Modules/SurgeiOS.swiftmodule/* SurgeiOS.framework/Modules/SurgeiOS.swiftmodule/
fi

lipo -create -output SurgeiOS.framework/SurgeiOS SurgeiOS.framework/SurgeiOS simulator/SurgeiOS.framework/SurgeiOS

rm -rf simulator

cd ..
zip -r apple-frameworks.zip iOS/SurgeiOS.framework/ macOS/SurgeMacOS.framework/
