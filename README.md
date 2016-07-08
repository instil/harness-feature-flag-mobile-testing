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

Prior to building the library ensure that you have cloned the project and checked out all its associated submodules.

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
$ mkdir apple/build
$ cd apple/build
$ cmake -DFOR_IOS=ON -G Xcode ../../core
```

### Android

To build the Android library, execute the following.

```bash
$ cd android
$ ./gradlew clean build
```

### Xamarin

TODO

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

## Testing streaming without a camera

If a camera is not available, we can also use the Gstreamer RTSP server with a test video source on macOS, Linux, etc. The following instructions are for macOS but the required packages are also available for Linux.

### Setup

Firstly, install GStreamer and the required dependencies.

```bash
$ brew install gstreamer gst-plugins-base gst-plugins-good gst-plugins-bad gst-plugins-ugly homebrew/versions/gst-ffmpeg010
```

Then build the RTSP server library and sample code we'll be using for testing.

```bash
$ mkdir ~/surge
$ cd ~/surge
$ wget https://gstreamer.freedesktop.org/src/gst-rtsp-server/gst-rtsp-server-1.4.4.tar.xz
$ tar xvf gst-rtsp-server-1.4.4.tar.xz
$ cd gst-rtsp-server-1.4.4
$ ./configure --prefix=/opt/gst-rtsp
$ make
```

### Streaming from test source

<!-- #### H264

```bash
$ cd ~/surge/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( videotestsrc ! video/x-raw, width=720, height=480, framerate=25/1 ! omxh264enc ! h264parse ! rtph264pay name=pay0 config-interval=1 pt=96 )"
``` -->

<!-- #### MPEG-4 Part 2

```bash
$ cd ~/surge/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( videotestsrc ! video/x-raw, width=720, height=480, framerate=25/1 ! ffenc_mpeg4 ! rtpmp4vpay name=pay0 config-interval=1 pt=96 )"
``` -->

#### MJPEG

```bash
$ cd ~/surge/gst-rtsp-server-1.4.4/examples
$ ./test-launch "( videotestsrc ! video/x-raw, width=720, height=480, framerate=25/1 ! jpegenc ! rtpjpegpay name=pay0 config-interval=1 pt=96 )"
```
