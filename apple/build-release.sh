#!/usr/bin/env bash

set -e

cd $(dirname $0)

echo "Executing tests"
#xctool -workspace Surge.xcworkspace -scheme SurgeCore run-tests \
#  -reporter pretty \
#  -reporter junit:build/test-results/core-lib-tests.xml

#xctool -workspace Surge.xcworkspace -scheme SurgeiOS run-tests \
#  -reporter pretty \
#  -reporter junit:build/test-results/framework-tests.xml

echo "Building macOS framework"
xcodebuild -workspace Surge.xcworkspace -scheme SurgeMacOS -configuration Release clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/macOS

echo "Building iOS framework"
xcodebuild -workspace Surge.xcworkspace -scheme SurgeiOS -configuration Release -sdk iphoneos clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/iOS

xcodebuild -workspace Surge.xcworkspace -scheme SurgeiOS -configuration Release -sdk iphonesimulator clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/iOS/simulator

cd build/frameworks/iOS

if [ -d "simulator/SurgeiOS.framework/Modules/SurgeiOS.swiftmodule" ]; then
  cp -r simulator/SurgeiOS.framework/Modules/SurgeiOS.swiftmodule/* SurgeiOS.framework/Modules/SurgeiOS.swiftmodule/
fi

lipo -create -output SurgeiOS.framework/SurgeiOS SurgeiOS.framework/SurgeiOS simulator/SurgeiOS.framework/SurgeiOS

rm -rf simulator

cd ..
zip -r apple-frameworks.zip iOS/SurgeiOS.framework/ macOS/SurgeMacOS.framework/
