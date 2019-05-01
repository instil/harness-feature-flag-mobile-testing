## Changelog

### v1.2.3

###### Fixes

* Await completion of TEARDOWN request before deallocating RtspClient.

### v1.2.2

###### Fixes

* Removed a retain cycle in the custom authentication logic on iOS.
* Fixed a file descriptor leak relating to a change in functionality in the upgrade in the libuv dependency.
* Changed the MJPEG depacketizer to take into account certain MJPEG streams that do not contain their quantization tables in the headers.
* Fixed a session description parsing issue for ONVIF cameras.
* Fixed a number of dealloc related crashes.

### v1.2.1

###### Fixes

* Updated the configuration used to build the Surge OpenSSL dependency on iOS to avoid triggering a non-public API usage rejection when submitting to the app store.
* Fixes a crash in RtspService cause when a TEARDOWN request fails.
* Fixed an issue with changing the start/end time on a stream once an initial start/end time has been set.
* Changed how we handle JNIEnv objects on Android to help with crashes relating to native thread not detaching from the JVM correctly or holding onto local references indefinitely.
* Changed the `initiatePlaybackOf` method to return the result of the `PLAY` call rather than the `SETUP` response.

###### Misc

* Readded previously removed SurgeCore API methods used in Tyco Surge.

### v1.2

###### Features

* TLS encryption - Added support for streaming TLS encrypted RTSP streams over TCP, triggered using `rtsps://` as the stream protocol.
* Added the ability for the user to provide a custom authenticator, used to inject headers into the RTSP requests to any arbitrary data through the socket.
* Digest authentication support, supporting the standard RFC 2069 implementation and all the MD5 variants defined in RFC 2617.

###### Fixes

* Fixed a memory leak within the H264 decoder.
* Fixed a memory leak with cleaning up after processing a frame of data, at the core level.

###### Misc

* Updated the libuv dependency to v1.23.1.
* Major refactor to the Core RTSP Client, reorganising code into smaller, easier to read and more easily testable classes.


### v1.1.2

###### Features

* Core: External UDP streams are now fully supported; Surge will now punch a hole in the NAT to ensure video data correctly gets from the source to Surge.
* iOS: Delegate calls are now called on the main thread.
* Core: Added an RTSP request timeout timer at the Transport level to detect a timeout on RTSP requests.
* Core: Added a dispatch queue for all outgoing callbacks from Core to avoid external work being done on the Transport thread, improving smoothness in stream playback.

###### Fixes

* Audio session description causing video session description to fail to parse fix.
* Fixed issue in the transports that caused data to sometimes be left behind after a stream had stopped, causing RTSP responses to appear corrupt when trying to reuse the player.
* Fixed issue that caused start and end time to not be cleared when trying to reuse a player to play stream from live.
* Fixed a file descriptor leak coming from libuv not cleaning up handles when they are closed.
* Various smaller JNI related Android stability fixes.

### v1.1.1

 * Added a RTP packet buffer to help handle out of order and lost packets with the UDP transport.
 * Additional error information now passed through to the client.
 * Updated the public API to return enum based error responses.

### v1.1

 * Added UDP support.

### v1.0

 * Initial release.
 * Supports H264, MP4V-ES and MJPEG decoding.
 * Supports the Interleaved TCP transport protocol.
 * Can be built for iOS, Mac OS, Android, Xamarin iOS and Xamarin Android.
