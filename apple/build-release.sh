#!/usr/bin/env bash

cd $(dirname $0)

mkdir build
cd build
cmake -DFOR_IOS=ON -G Xcode ../../core

cd ..
echo "Building iOS framework"
xcodebuild -workspace Surge.xcworkspace -scheme SurgeiOS -configuration Release clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/iOS/release \
  CODE_SIGN_IDENTITY="iPhone Developer: Instil Software"

echo "Building macOS framework"
xcodebuild -workspace Surge.xcworkspace -scheme SurgeMacOS -configuration Release clean build \
  CONFIGURATION_BUILD_DIR=$(pwd)/build/frameworks/macOS/release \
  CODE_SIGN_IDENTITY="iPhone Developer: Instil Software"

cd build/frameworks
zip -r apple-frameworks.zip .
