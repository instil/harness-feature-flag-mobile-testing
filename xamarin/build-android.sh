#!/bin/bash

BUILD_DIR=$(pwd)/SurgeXamarinAndroid/SurgeXamarinAndroidBindings/Jars
SURGE_DIR=../android

rm $BUILD_DIR/SurgeAndroid.aar

cd $SURGE_DIR

./gradlew copyLibs
./gradlew clean surge:build -x checkstyle -x findbugs
./gradlew cleanLibs

cp $SURGE_DIR/surge/build/outputs/aar/surge-release.aar $BUILD_DIR/SurgeAndroid.aar
