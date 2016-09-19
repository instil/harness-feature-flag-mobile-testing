#!/bin/bash

XAMARIN_DIR=$(pwd)
BUILD_DIR=$XAMARIN_DIR/SurgeXamarinAndroid/SurgeXamarinAndroidBindings/Jars
SURGE_DIR=../android

rm $BUILD_DIR/SurgeAndroid.aar

cd $SURGE_DIR

./gradlew copyLibs
./gradlew clean surge:build -x checkstyle -x findbugs
./gradlew cleanLibs

cp $SURGE_DIR/surge/build/outputs/aar/surge-release.aar $BUILD_DIR/SurgeAndroid.aar

xbuild /p:Configuration=Release $XAMARIN_DIR/SurgeXamarinAndroid/SurgeXamarinAndroid.sln
