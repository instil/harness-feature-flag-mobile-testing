# Surge

Surge is a high performance Real-Time Streaming Protocol ([RTSP](https://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol)) and Real-Time Transport Protocol ([RTP](https://en.wikipedia.org/wiki/Real-time_Transport_Protocol)) implementation with support for multiple languages and platforms. RTP can handle many AV formats and Surge currently supports the most popular formats below.

- H264
- MP4V-ES
- MJPEG

RTSP and RTP are currently a popular choice for use with IP video cameras both commercially and for home/hobbyist users e.g. the Raspberry Pi camera. However, real-time streaming of video data poses multiple challenges:

- Real-time encoding/decoding (particularly on lower end hardware like the RPi and mobile devices).
- Minimizing client/server latency.

Surge seeks to address these issues by being designed from the ground up to execute asynchronously whilst utilising non-blocking I/O and leveraging platform specific APIs to access hardware accelerated video decoding.

## Release Notes

### v1.1.1

 * Added a RTP packet buffer to help handle out of order and lost packets with the UDP transport
 * Additonal error informtion now passed through to the client

### v1.1

 * Added UDP support

### v1.0

 * Initial release
 * Supports H264, MP4V-ES and MJPEG decoding
 * Supports the Interleaved TCP transport protocol
 * Can be built for iOS, Mac OS, Android, Xamarin iOS and Xamarin Android

## Development

### Apple Frameworks

A workspace is located at `apple/Surge.xcworkspace` for developing any Apple Surge frameworks, including iOS, macOS and tvOS. Frameworks are built using an embedded Xcode project and their products are placed in `apple/DerivedData` and automatically linked within the relevant demo apps in the workspace. All derived data (including built framework products) is omitted from source conrol by default.

### Android Frameworks

A Gradle project is located under `android/build.gradle`, which contains the Android Surge project containing the decoder code and JNI bindings to the SurgeCore project. 


## Building

Prior to building the library ensure that you have cloned the project and checked out all its associated submodules.

```bash
$ git submodule update --init --recursive
```

### iOS

To build the iOS/macOS frameworks, either open the workspace `apple/Surge.xcworkspace` in XCode or execute the following.

```bash
$ cd apple
$ ./build-release.sh
```

### Android

To build the Android library, execute the following.

```bash
$ cd android
$ ./gradlew clean build
```

### Xamarin

Ensure you have built either or both of the frameworks you are wishing to target using the above steps per platform. Then execute one of the following build commands for the platform of your choice.

```bash
$ cd xamarin
$ ./build-ios.sh
```

```bash
$ cd xamarin
$ ./build-android.sh
```


### Core

To build the core Surge C/C++ library.

```bash
$ brew install cmake
$ mkdir core/build
$ cd core/build
$ cmake -DUNIT_TESTS=ON -DCODE_COVERAGE=ON -DDEBUG_SYMBOLS=ON ../
$ make
```

To execute the tests.

```bash
$ ./test/testrunner
```

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
$ sudo wget https://gstreamer.freedesktop.org/src/gst-rtsp-server/gst-rtsp-server-1.4.4.tar.xz
$ sudo tar xvf gst-rtsp-server-1.4.4.tar.xz
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
$ sudo brew install gstreamer gst-plugins-base gst-plugins-good gst-plugins-bad gst-plugins-ugly
```

Then build the RTSP server library and sample code we'll be using for testing.

```bash
$ mkdir ~/surge
$ cd ~/surge
$ sudo wget https://gstreamer.freedesktop.org/src/gst-rtsp-server/gst-rtsp-server-1.4.4.tar.xz
$ sudo tar xvf gst-rtsp-server-1.4.4.tar.xz
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

## Expose a stream via `ngrok`

Once you have a stream up and running, you can expose it outside the network via `ngrok`. This is useful for testing on potentially captive networks or for testing on mobile devices over cellular, or where device connection to the same network as the stream is not desired / not possible.

The `ngrok` utility is simple to install and quick to run. Links to the latest Linux ARM build can be located on [the downloads page](https://ngrok.com/download) but is unlikely to change from the instructions below.

To install on the Raspberry Pi:

```bash
$ cd ~
$ sudo wget https://bin.equinox.io/c/4VmDzA7iaHb/ngrok-stable-linux-arm.zip
$ unzip ngrok-stable-linux-arm.zip
$ sudo mv ngrok /usr/local/bin
$ sudo rm ngrok-stable-linux-arm.zip
```

Once the utility is installed you will first need to add your authentication to any `ngrok` commands. Use of the service is free and there are many ways to signup, including authorisation through your GitHub account. Once you have a token, authorise your `ngrok` install:

```bash
$ ngrok authtoken <YOUR_AUTHTOKEN>
```

Now opening a TCP tunnel to local port `8554` is very simple:

```bash
$ ngrok tcp -region eu 8554
```

And the output should look something like this:

```
Version                       2.2.8
Region                        Europe (eu)
Web Interface                 http://127.0.0.1:4040
Forwarding                    tcp://0.tcp.eu.ngrok.io:<PORT> -> localhost:8554
```

When the Gstreamer RTSP server is running on port `8554` the stream can now be accessed externally by adapting the URL to `rtsp://0.tcp.eu.ngrok.io:<PORT>/test`.

