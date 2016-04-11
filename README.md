# Surge

High performance RTSP implementation with libraries/frameworks/wrappers for iOS, Android, C# and Python. 

## Background

RTSP is a real time streaming protocol with support for multiple Audio/Video (AV) formats which is currently used heavily in surveillance equipment.
Real-time streaming of video data poses multiple challenges:

- Real-time encoding/decoding.
- Minimizing client/server latency.

<!-- Surge seeks to address these issues by executing asynchronously 
uses abstract events to run asynchronously and reduce cpu load as much as possible.
Payloads and events are handled in the client via a Delegate interface. -->

RTP can handle many AV formats and Surge currently supports the most common formats below. Where possible Surge will attempt to use hardware accelerated decoding using platform specific APIs.

- H264 RTP Payloads.
- MP4V-ES RTP Payloads.
- MJPEG RTP Payloads.

## Compilation

Using [CMake](https://cmake.org/) allows us to generate IDE specific project files. This is used to port the code to iOS. To build for local development:

```bash
$ brew install cmake ffmpeg
$ mkdir build
$ cd build
$ cmake -DUNIT_TESTS=ON -DDEBUG_SYMBOLS=ON ../
$ make
```

Development RTSP Server - This example serves the Serenity trailer.

```bash
$ ffserver -f ffserver.conf
```

Run Tests:

```bash
$ ./test/testrunner
```

### Usage

To play an RTSP stream a Client must issues a DESCRIBE -> SETUP -> PLAY.

- DESCRIBE: Asks the server for what AV data is available. Which is defined by the Session Description Protocol in the describe response body. Describe responses will return this "_palette_".
- SETUP: Takes a palette and setups up a session and transport between the client and the server for the selected palette. Palettes contain a control url which is used to issue this setup request.
- PLAY: Uses the session and transport defined from the SETUP response to handle the stream.
- PAUSE: Pauses the stream.
- OPTIONS: Asks the server for what methods are available (DESCRIBE/SETUP/PLAY/GET_PARAMETER/...)
- TEARDOWN: Will destroy a specified session.

### Debugging

ScratchTest.cc is a test file for playing with usage of the library and i run:

```bash
$ lldb -- ./test/testrunner --gtest_filter=SCRATCH.SIMPLE_SCRATCH
```

### Profiling

To profile i use Valgrind:

```bash
$ brew install --HEAD valgrind
$ valgrind -- ./test/testrunner
```

## Ports and Bindings

Surge is designed from scratch to work on iOS and Android at the very least. Python bindings are available and a c-wrapper which can be used for xamarin PInvoke.

### iOS

There is an iOS Integration Test within IntegrationTests/. To build a release library for iOS:

```bash
$ ./release-ios.sh
```

### Android

To compile on android i use the android-cmake toolchain. It is already in the project with some modifications. Namely the -fstrict-aliasing needed to be
removed as this generates bad code for SWIG Directors which will crash.

You also need the Android-NDK to be installed.

```bash
$ export ANDROID_NDK=~/android-ndk-r10e
$ ./release-android.sh
```

### Python Bindings

TODO

### C Wrapper (PInvoke)

TODO

### Podspec

TODO

## Raspberry Pi - Real-Time Camera

The Raspberry Pi 2+ is capable of hardware accelerated video encoding/decoding through usage of the OpenMAX (OMX) APIs. Broadcom also provide the Multi-Media Abstraction Layer (MMAL) API which is a layer on top of OMX intended to provided a simpler API. 

Unfortunately ffmpeg does not use OMX and falls back to using software encoding which is too slow to be used in real-time processing. Tests on Raspberry Pi shown that it couldn't do much better than 0.2 speed so it eventually gets out of sync. Gstreamer however does allow use of the OMX APIs so we can use that for testing real-time transmission of video from the Raspberry Pi.

To use the gstreamer RTSP server:

```bash
$ sudo apt-get install gstreamer1.0 gstreamer1.0-tools gstreamer1.0-omx libgstreamer1.0-dev
$ wget https://gstreamer.freedesktop.org/src/gst-rtsp-server/gst-rtsp-server-1.4.4.tar.xz
$ tar xvf gst-rtsp-server-1.4.4
$ cd gst-rtsp-server
$ ./configure --prefix=/opt/gst-rtsp
$ make
$ sudo modprobe bcm2835-v4l2
$ cd examples
$ ./test-launch "( v4l2src device=/dev/video0 ! omxh264enc ! video/x-h264,width=720,height=480,framerate=10/1,profile=high,target-bitrate=8000000 ! h264parse ! rtph264pay name=pay0 config-interval=1 pt=96 )"
```

Connect Surge to: rtsp://raspberry-pi-ip:8554/test
