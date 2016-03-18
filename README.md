# Surge

RTSP implementation from scratch.

Currently implements RTSP Interleaved and handles SessionDescriptionV0 protocol and ignores RTCP. Only H264 depacketization so far.

## Compilation

To build for local development:

```bash
$ brew install cmake ffmpeg
$ mkdir build
$ cd build
$ cmake -DUNIT_TESTS=ON -DDEBUG_SYMBOLS=ON ../
$ make
```

Development RTSP Server

```bash
$ ffserver -f ffserver.conf
```

Run Tests:

```bash
$ ./test/testrunner
```

### XCode

If you prefer to use Xcode to develop use:

```bash
$ cmake -DUNIT_TESTS=ON -DDEBUG_SYMBOLS=ON -GXcode ../
```

Though the cmake build will need to be fixed/changed so tests will compile. This is broken because
of external project fetching (gtest/gmock).

### Clion

CLion automatically supports CMake projects out of the box. Other IDE support avaialble throught cmake generators see:

```bash
$ cmake --help
```

Unittesting will work from clion!

## Debugging

ScratchTest.cc is a test file for playing with usage of the library and i run:

```bash
$ lldb -- ./test/testrunner --gtest_filter=SCRATCH.SIMPLE_SCRATCH
```

## Profiling

To profile i use Valgrind:

```bash
$ brew install --HEAD valgrind
$ valgrind -- ./test/testrunner
```

## iOS

There is an iOS Integration Test within IntegrationTests/. To build a release library for iOS:

```bash
$ ./release-ios.sh
```

## Android

To compile on android i use the android-cmake toolchain. It is already in the project with some modifications. Namely the -fstrict-aliasing needed to be
removed as this generates bad code for SWIG Directors which will crash.

You also need the Android-NDK to be installed.

```bash
$ export ANDROID_NDK=~/android-ndk-r10e
$ ./release-android.sh
```

## Python Bindings

Python bindings are available:

```bash
$ python ./setup.py build
$ sudo python ./setup.py install
```

# Raspberry Pi - Real-Time Camera

The raspberry pi is a beast of a wee machine. To view video in real time the server _must_ use the hardware h264 encoder provided by OMX framework.

Unfortunatly ffmpeg does not use OMX and falls back to using software encoding which is too slow to be in realtime. Tests on raspberry pi shown that it couldn't do much better than 0.2 speed so it eventually gets incredibly out of sync.

```bash
$ sudo apt-get install gstreamer1.0 gstreamer1.0-tools gstreamer1.0-omx libgstreamer1.0-dev 
$ wget https://gstreamer.freedesktop.org/src/gst-rtsp-server/gst-rtsp-server-1.4.4.tar.xz
$ tar xvf gst-rtsp-server-1.4.4
$ cd gst-rtsp-server
$ ./configure --prefix=/opt/gst-rtsp
$ make
$ sudo modprobe bcm2835-v4l2
$ cd examples
$ ./test-launch "( v4l2src device=/dev/video0 ! omxh264enc ! video/x-h264,width=720,height=480,framerate=10/1,profile=high,target-bitrate=5000000 ! h264parse ! rtph264pay name=pay0 config-interval=1 pt=96 )"
```
