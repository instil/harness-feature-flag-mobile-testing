#!/bin/bash

XAMARIN_DIR=$(pwd)
SURGE_DIR=../apple
BUILD_DIR=$XAMARIN_DIR/SurgeXamariniOS/lib

rm -r $BUILD_DIR
mkdir $BUILD_DIR

cd $SURGE_DIR

$SURGE_DIR/build-release.sh
cp -r $SURGE_DIR/build/frameworks/iOS/SurgeiOS.framework $BUILD_DIR/
cp -r $SURGE_DIR/build/frameworks/iOS/libSurgeCore.a $BUILD_DIR/

"/Applications/Xamarin Studio.app/Contents/MacOS/mdtool" -v build "--configuration:Release" $XAMARIN_DIR/SurgeXamariniOS/SurgeXamariniOS.sln
