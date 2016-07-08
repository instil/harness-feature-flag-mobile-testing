#!/usr/bin/env bash

cd $(dirname $0)

mkdir build
cd build
cmake -DFOR_IOS=ON -G Xcode ../../core

cd ..
echo "Building iOS framework"
xcodebuild -workspace Surge.xcworkspace -scheme SurgeiOS -configuration Release clean build \
  TARGET_BUILD_DIR=$(pwd)/build/ios-framework

echo "Building macOS framework"
xcodebuild -workspace Surge.xcworkspace -scheme SurgeMacOS -configuration Release clean build \
  TARGET_BUILD_DIR=$(pwd)/build/macos-framework
