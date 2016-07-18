#!/bin/bash

SURGE_DIR=../apple
BUILD_DIR=$(pwd)/SurgeXamariniOS/lib

rm -r $BUILD_DIR/SurgeiOS*.framework
rm $BUILD_DIR/libsurge.a

cd $SURGE_DIR

$SURGE_DIR/build-release.sh
cp -r $SURGE_DIR/build/frameworks/SurgeiOS.framework $BUILD_DIR/
cp -r $SURGE_DIR/build/frameworks/libsurge.a $BUILD_DIR/
