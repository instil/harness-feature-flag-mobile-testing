## Release Notes

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