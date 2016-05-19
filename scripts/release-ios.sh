#!/usr/bin/env bash
set -e
. ./version.sh

rm -rf _xcode
mkdir _xcode
pushd _xcode

cmake -DFOR_IOS=ON -G Xcode ../

export ONLY_ACTIVE_ARCH=NO
# armv7
xcodebuild -project Surge.xcodeproj -target surge -sdk iphoneos ARCHS='armv7' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-armv7' BUILT_PRODUCTS_DIR='./_xcode/build-armv7'
# armv7s
xcodebuild -project Surge.xcodeproj -target surge -sdk iphoneos ARCHS='armv7s' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-armv7s' BUILT_PRODUCTS_DIR='./_xcode/build-armv7s'
# arm64
xcodebuild -project Surge.xcodeproj -target surge -sdk iphoneos ARCHS='arm64' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-arm64' BUILT_PRODUCTS_DIR='./_xcode/build-arm64'
# i386
xcodebuild -project Surge.xcodeproj -target surge -sdk iphonesimulator ARCHS='i386' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-i386' BUILT_PRODUCTS_DIR='./_xcode/build-i386'
# x86_64
xcodebuild -project Surge.xcodeproj -target surge -sdk iphonesimulator ARCHS='x86_64' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-x86_64' BUILT_PRODUCTS_DIR='./_xcode/build-x86_64'
lipo -create \
     './build-armv7/libsurge.a' \
     './build-armv7s/libsurge.a' \
     './build-arm64/libsurge.a' \
     './build-i386/libsurge.a' \
     './build-x86_64/libsurge.a' \
     -output 'libsurge.a'
popd

# create library archive with headers.
echo "Creating library archive: librtsp-ios.tar.gz..."

rm -rf librtsp-ios librtsp-ios.tar.gz
mkdir librtsp-ios
mkdir librtsp-ios/lib
mkdir librtsp-ios/include

pushd src
    for header in `find . -name "*.h"`; do
        cp $header "../librtsp-ios/include/"
    done
popd
cp -v _xcode/libsurge.a librtsp-ios/lib
rm -rf _xcode

pushd librtsp-ios
touch $GENERATED_FILE_IOS

echo "# Surge RTSP iOS" >> $GENERATED_FILE_IOS
echo "libsurge version: $VERSION" >> $GENERATED_FILE_IOS
echo "Builder: $BUILDER - $CURRENT_TIME" >> $GENERATED_FILE_IOS
echo "" >> $GENERATED_FILE_IOS
echo "Supported systems:" >> $GENERATED_FILE_IOS
echo "* iphoneos - armv7" >> $GENERATED_FILE_IOS
echo "* iphoneos - armv7s" >> $GENERATED_FILE_IOS
echo "* iphoneos - arm64" >> $GENERATED_FILE_IOS
echo "* iphonesimulator - i386" >> $GENERATED_FILE_IOS
echo "* iphonesimulator - x86_64" >> $GENERATED_FILE_IOS
popd

tar zcf librtsp-ios.tar.gz librtsp-ios/

echo "Done.."
