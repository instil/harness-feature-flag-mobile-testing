#!/usr/bin/env bash
set -e
. ./version.sh

: ${ANDROID_NDK:?"Need to set the ANDROID_NDK path envrioment variable"}

ARCHS=("armeabi-v7a"
       "armeabi" \
       "arm64-v8a" \
       "x86" \
       "x86_64")

pushd src
echo "Generating swig jni wrapper"
rm -f jni/*.java
rm -f jni/interface_wrap.*
swig -c++ -java -cppext cpp -package com.tyco.rtsp.android -Wall jni/interface.i
popd

# Build JNI Bindings Jar
rm -rf _android_bindings
mkdir _android_bindings
pushd _android_bindings
cmake -DBUILD_JAR=ON ../
make VERBOSE=1
popd

# Build JNI Native libs
for arch in "${ARCHS[@]}"; do
    rm -rf "_android_$arch"
    mkdir "_android_$arch"
    pushd "_android_$arch"
    
    echo "Building for [$arch]"
    echo

    cmake -DFOR_ANDROID=ON -DBUILD_JNI_WRAPPER=ON \
	  -DCMAKE_TOOLCHAIN_FILE=../android-cmake/android.toolchain.cmake \
	  -DCMAKE_BUILD_TYPE=Release \
          -DANDROID_STL=gnustl_static \
          -DANDROID_NATIVE_API_LEVEL=android-17 \
	  -DANDROID_ABI="$arch" \
	  ../
    make -j2 VERBOSE=1
    
    popd
done

echo "Creating library archive: librtsp-android.tar.gz..."

rm -rf librtsp-android librtsp-android.tar.gz
mkdir librtsp-android
mkdir -p librtsp-android/lib

cp -v _android_bindings/src/VideoEdgeRtsp.jar librtsp-android/
rm -rf _android_bindings
for arch in "${ARCHS[@]}"; do
    mkdir librtsp-android/lib/$arch
    cp -v "_android_$arch/src/libVideoEdgeRtsp.so" "librtsp-android/lib/$arch/"
    rm -rf "_android_$arch"
done

pushd librtsp-android
touch $GENERATED_FILE_ANDROID

echo "# RTSP Android" >> $GENERATED_FILE_ANDROID
echo "librtsp version: $VERSION" >> $GENERATED_FILE_ANDROID
echo "Builder: $BUILDER - $CURRENT_TIME" >> $GENERATED_FILE_ANDROID
echo "" >> $GENERATED_FILE_ANDROID
echo "Supported systems:" >> $GENERATED_FILE_ANDROID
for arch in "${ARCHS[@]}"; do
    echo "* Android - $arch" >> $GENERATED_FILE_ANDROID
done
popd

tar zcf librtsp-android.tar.gz librtsp-android

echo "Done.."
