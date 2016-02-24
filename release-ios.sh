#!/usr/bin/env bash
set -e
. ./version.sh

rm -rf _xcode
mkdir _xcode
pushd _xcode

cmake -DFOR_IOS=ON -G Xcode ../

export ONLY_ACTIVE_ARCH=NO
# armv7
xcodebuild -project TycoRTSP.xcodeproj -target VideoEdgeRtsp -sdk iphoneos ARCHS='armv7' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-armv7' BUILT_PRODUCTS_DIR='./_xcode/build-armv7'
# armv7s
xcodebuild -project TycoRTSP.xcodeproj -target VideoEdgeRtsp -sdk iphoneos ARCHS='armv7s' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-armv7s' BUILT_PRODUCTS_DIR='./_xcode/build-armv7s'
# arm64
xcodebuild -project TycoRTSP.xcodeproj -target VideoEdgeRtsp -sdk iphoneos ARCHS='arm64' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-arm64' BUILT_PRODUCTS_DIR='./_xcode/build-arm64'
# i386
xcodebuild -project TycoRTSP.xcodeproj -target VideoEdgeRtsp -sdk iphonesimulator ARCHS='i386' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-i386' BUILT_PRODUCTS_DIR='./_xcode/build-i386'
# x86_64
xcodebuild -project TycoRTSP.xcodeproj -target VideoEdgeRtsp -sdk iphonesimulator ARCHS='x86_64' \
           -configuration Release clean build TARGET_BUILD_DIR='./_xcode/build-x86_64' BUILT_PRODUCTS_DIR='./_xcode/build-x86_64'
lipo -create \
     './build-armv7/libVideoEdgeRtsp.a' \
     './build-armv7s/libVideoEdgeRtsp.a' \
     './build-arm64/libVideoEdgeRtsp.a' \
     './build-i386/libVideoEdgeRtsp.a' \
     './build-x86_64/libVideoEdgeRtsp.a' \
     -output 'libVideoEdgeRtsp.a'
popd

# create library archive with headers.
echo "Creating library archive: librtsp-ios.tar.gz..."

rm -rf librtsp-ios librtsp-ios.tar.gz
mkdir librtsp-ios
mkdir librtsp-ios/lib
mkdir librtsp-ios/include

headers=("RTSP" "TycoAuxiliary" "TycoUtility" "jRTPlib" "wrapper")
for item in ${headers[*]}; do
    mkdir "librtsp-ios/include/$item"
done

pushd src
for item in ${headers[*]}; do
    echo "Adding headers from [$item] to archive"
    for header in `find $item -name "*.h"`; do
        cp $header "../librtsp-ios/include/$item/"
    done
done
popd
cp -v _xcode/libVideoEdgeRtsp.a librtsp-ios/lib
rm -rf _xcode

pushd librtsp-ios
touch $GENERATED_FILE_IOS

echo "# RTSP iOS" >> $GENERATED_FILE_IOS
echo "librtsp version: $VERSION" >> $GENERATED_FILE_IOS
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
