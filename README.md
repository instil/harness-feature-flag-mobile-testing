# Surge

Surge is a high performance Real-Time Streaming Protocol ([RTSP](https://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol)) and Real-Time Transport Protocol ([RTP](https://en.wikipedia.org/wiki/Real-time_Transport_Protocol)) implementation with support for multiple languages and platforms. RTP can handle many AV formats and Surge currently supports the most popular formats below.

- H264
- MP4V-ES
- MJPEG

RTSP and RTP are currently a popular choice for use with IP video cameras both commercially and for home/hobbyist users e.g. the Raspberry Pi camera. However, real-time streaming of video data poses multiple challenges:

- Real-time encoding/decoding (particularly on lower end hardware like the RPi and mobile devices).
- Minimizing client/server latency.

Surge seeks to address these issues by being designed from the ground up to execute asynchronously whilst utilising non-blocking I/O and leveraging platform specific APIs to access hardware accelerated video decoding. 

## Building

### Core

To build the core Surge C/C++ library.

```bash
$ brew install cmake
$ mkdir core/build
$ cd core/build
$ cmake -DUNIT_TESTS=ON -DDEBUG_SYMBOLS=ON ../
$ make
```

To execute the tests.

```bash
$ ./test/testrunner
```

### iOS

To build the iOS frameworks, first generate the Xcode project files for the Surge core library then open the workspace in Xcode.

```bash
$ brew install cmake
$ mkdir core/build
$ cd core/build
$ cmake -DUNIT_TESTS=ON -DDEBUG_SYMBOLS=ON ../
```

### Android

To build the Android library, execute the following. 

```bash
$ cd android
$ ./gradlew clean build
```

### Xamarin

TODO

### Python

TODO

<!--

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
```-->

## Testing real-time streaming

The Raspberry Pi 2+ and camera module provide a cheap option for testing real-time streaming which is capable of hardware accelerated H264 video encoding/decoding through usage of the OpenMAX (OMX) APIs. Broadcom in addition also provide the Multi-Media Abstraction Layer (MMAL) API on top of OMX which intended to provide a simpler API for devices with the VideoCore GPU (and this is actually what the raspivid utility uses for hardware accelerate H264 encoding). There are a number of options for live-streaming the RPi camera using RTSP but unfortunately not all of these options support the full feature set that we require for testing Surge - capturing compressed video data using the raspivid utility and packetizing it with the in-built VLC RTSP server works well but does not support interleaved RTP over TCP. Gstreamer however does allow use of the OMX APIs and provides an RTSP library with some helpful examples for building an RTSP server which we can use for testing. 

The instructions below have been tested on a Raspberry Pi but they should work for any Debian based system with access to a camera and the appropriate Video4Linux driver. It's also possible to use the GStreamer test source if a camera is not available, examples of which are provided below.

### Setup

Firstly, install GStreamer and the required dependencies. The script `tools/raspberry-pi-setup.sh` can also be used to automate this process for a Raspberry Pi.

```bash
$ sudo apt-get install gstreamer1.0 gstreamer1.0-tools gstreamer1.0-omx libgstreamer1.0-dev v4l-utils
```

Next, load the Broadcom Video4Linux driver for the Raspberry Pi which we'll be using. For other platforms, substitute with the appropriate driver for your camera.

```bash
$ sudo bash -c "echo bcm2835-v4l2 >> /etc/modules"
$ sudo modprobe bcm2835-v4l2
```

Then build the RTSP server library and sample code we'll be using for testing.

```bash
$ cd /usr/src
$ wget https://gstreamer.freedesktop.org/src/gst-rtsp-server/gst-rtsp-server-1.4.4.tar.xz
$ tar xvf gst-rtsp-server-1.4.4.tar.xz
$ cd gst-rtsp-server-1.4.4
$ ./configure --prefix=/opt/gst-rtsp
$ make
```

### Streaming from live camera

#### H264

```bash
$ cd /usr/src/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( v4l2src device=/dev/video0 ! omxh264enc ! video/x-h264,width=720,height=480,framerate=25/1,profile=high,target-bitrate=8000000 ! h264parse ! rtph264pay name=pay0 config-interval=1 pt=96 )"
```

#### MPEG-4 Part 2

```bash
$ cd /usr/src/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( v4l2src device=/dev/video0 extra-controls=\"c,video_bitrate=8000000\" ! video/x-raw, width=720, height=480, framerate=25/1 ! videoconvert ! avenc_mpeg4 ! rtpmp4vpay name=pay0 config-interval=1 pt=96 )"
```

#### MJPEG

```bash
$ cd /usr/src/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( v4l2src device=/dev/video0 extra-controls=\"c,video_bitrate=8000000\" ! video/x-raw, width=720, height=480, framerate=25/1 ! videoconvert ! jpegenc ! rtpjpegpay name=pay0 config-interval=1 pt=96 )"
```

### Streaming from GStreamer test source

#### H264

```bash
$ cd /usr/src/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( videotestsrc ! video/x-raw, width=720, height=480, framerate=25/1 ! omxh264enc ! h264parse ! rtph264pay name=pay0 config-interval=1 pt=96 )"
```

#### MPEG-4 Part 2

```bash
$ cd /usr/src/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( videotestsrc ! video/x-raw, width=720, height=480, framerate=25/1 ! avenc_mpeg4 ! rtpmp4vpay name=pay0 config-interval=1 pt=96 )"
```

#### MJPEG

```bash
$ cd /usr/src/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( videotestsrc ! video/x-raw, width=720, height=480, framerate=25/1 ! jpegenc ! rtpjpegpay name=pay0 config-interval=1 pt=96 )"
```