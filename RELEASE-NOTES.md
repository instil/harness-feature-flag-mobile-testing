## Release Notes

### v1.1.2

###### Features

* Core: External UDP streams are now supported; Surge will now puch a hole in the NAT to ensure video dta correctly gets from the source to Surge.
* iOS: Delegate calls are now called on the main thread.
* Core: Added an RTSP request timeout timer at the Transport level to detect a timeout on RTSP requests.

###### Fixes

* Audio session description causing video SD to fail to parse fix.
* Fixed issue in the transports that caused data to sometimes be left behind after a stream had stopped, causing RTSP responses to appear corrupt when trying to reuse the player.
* Fixed issue that caused start and end time to not be cleared when trying to reuse a player to play stream from live.
* Fixed a file descriptor leak coming from libuv not cleaning up handles when they are closed.

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