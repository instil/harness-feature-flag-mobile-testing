using System;

using UIKit;
using Foundation;
using ObjCRuntime;
using CoreGraphics;
using System.Collections.Generic;

namespace SurgeXamariniOSBindings {
    /*! \mainpage Surge
     *
	 * Surge is a high performance Real-Time Streaming Protocol (RTSP) and Real-Time Transport Protocol (RTP) implementation with support for multiple languages and platforms. RTP can handle many AV formats and Surge currently supports the most popular formats below.
	 * 
	 * - H264
	 * - MP4V-ES
	 * - MJPEG
	 */

    /// <summary>
    /// The core class used to start and control playback of an RTSP stream on iOS.
    /// </summary>
    [BaseType(typeof(NSObject))]
    public interface SurgeRtspPlayer {
        /// <summary>
        /// Initiate playback of the RTSP stream.
        /// 
        /// This will issue the required DESCRIBE, SETUP and PLAY RTSP requests to start stream playback.
        /// Once completed, Surge will notify the user of the requests result via the Surge RTSP delegate.
        /// If successful, the video stream will start playing on the provided ImageView associated with the SurgeRtspPlayer#PlayerView property.
        /// 
        /// \sa SurgeRtspPlayerDelegate
        /// </summary>
        /// <param name="url">The RTSP stream URL.</param>
        [Export("initiatePlaybackOf:")]
        void InitiatePlaybackOf(NSUrl url);

        /// <summary>
        /// Initiate playback of a basic auth protected RTSP stream.
        /// 
        /// This will issue the required DESCRIBE, SETUP and PLAY RTSP requests to start stream playback.
        /// Once completed, Surge will notify the user of the requests result via the Surge RTSP delegate.
        /// If successful, the video stream will start playing on the provided ImageView associated with the SurgeRtspPlayer#PlayerView property.
        /// 
        /// \sa SurgeRtspPlayerDelegate
        /// </summary>
        /// <param name="url">The RTSP stream URL.</param>
        /// <param name="username">Username used to authenticate the stream.</param>
        /// <param name="password">Password associated with the provided username.</param>
        [Export("initiatePlaybackOf:withUsername:andPassword:")]
        void InitiatePlaybackOf(NSUrl url, NSString username, NSString password);

        /// <summary>
        /// Initiate playback of the a basic auth protected RTSP stream.
        /// 
        /// This will issue the required DESCRIBE, SETUP and PLAY RTSP requests to start stream playback.
        /// Once completed, Surge will notify the user of the requests result via the Surge RTSP delegate.
        /// If successful, the video stream will start playing on the provided ImageView associated with the SurgeRtspPlayer#PlayerView property.
        /// 
        /// \sa SurgeRtspPlayerDelegate
        /// </summary>
        /// <param name="url">The RTSP stream URL.</param>
        /// <param name="username">Username used to authenticate the stream.</param>
        /// <param name="password">Password associated with the provided username.</param>
        /// <param name="startTime">If the RTSP stream supports recorded video, timestamp to start playing video from.</param>
        /// <param name="endTime">If the RTSP stream supports recorded video, timestamp to finish playing video at.</param>
        [Export("initiatePlaybackOf:withUsername:andPassword:startingAt:andEndingAt:")]
        void InitiatePlaybackOf(NSUrl url, NSString username, NSString password, NSDate startTime, [NullAllowed] NSDate endTime);

        /// <summary>
        /// Stop playback of the stream, issuing the TEARDOWN RTSP request.
        /// 
        /// Once Stop has been requested, the stream cannot be restarted via the Play() command. To restart a stream, please re-execute the InitiatePlaybackOf() command.
        /// </summary>
		[Export("stop")]
        void Stop();

        /// <summary>
        /// Pause playback of the stream.
        /// </summary>
        [Export("pause")]
        void Pause();

        /// <summary>
        /// Resumes playback of a paused stream.
        /// </summary>
        [Export("play")]
        void Play();

        /// <summary>
        /// The ImageView that Surge will use to play the RTSP video stream on.
        /// </summary>
        [Export("playerView")]
        UIImageView PlayerView { get; set; }

        /// <summary>
        /// Delegate object that will receive the notifications of events from the Surge library.
        /// 
        /// This is a weak property. A reference to the delegate object must be stored on the client 
        /// side to ensure it does not get garbage collected.
        /// </summary>
        [Export("delegate")]
        SurgeRtspPlayerDelegate ClientDelegate { get; set; }

        /// <summary>
        /// Optional delegate used to receive any diagnostics updates as they are calculated by the Diagnostics object.
        /// </summary>
        [Export("diagnosticsDelegate")]
        SurgeDiagnosticsDelegate DiagnosticsDelegate { get; set; }

        /// <summary>
        /// Item containing the latest diagnostics information for the currently playing stream.
        /// </summary>
        [Export("diagnostics")]
        ISurgeDiagnostics Diagnostics { get; }

        /// <summary>
        /// Frames per second measured from the currently playing stream.
        /// </summary>
        [Export("framesPerSecond")]
        int FramesPerSecond { get; }

        /// <summary>
        /// Maximum length of time, in milliseconds, between frames before Surge will issue a timeout signal.
        /// 
        /// Default value: 5000ms
        /// </summary>
        [Export("timeout")]
        int Timeout { get; set; }

        /// <summary>
        /// If true, Surge will stream video data via an interleaved TCP transport rather than via UDP
        /// </summary>
        [Export("interleavedTcpTransport")]
        bool InterleavedTcpTransport { get; set; }

        /// <summary>
        /// Time delay, in milliseconds, between Surge receiving a packet containing video data and the 
        /// frame data being decoded for viewing. Used for error correction and packet reordering 
        /// purposes for the UDP transport. 
        /// 
        /// Default: 200ms
        /// </summary>
        [Export("packetBufferDelay")]
        int PacketBufferDelay { get; set; }

        /// <summary>
        /// Enables/disables TLS certificate validation when streaming from a TLS protected RTSP stream. Default: true
        /// </summary>
        /// <value><c>true</c> if TLS certificate verification is enabled; otherwise, <c>false</c>.</value>
        [Export("tlsCertificateValidationEnabled")]
        bool TlsCertificateValidationEnabled { get; set; }

        /// <summary>
        /// Allows/disallows self signed TLS certificates from being used when streaming from a TLS protected RTSP stream. Default: true
        /// </summary>
        /// <value><c>true</c> if a TLS self signed certificate is allowed; otherwise, <c>false</c>.</value>
        [Export("tlsSelfSignedCertificateAllowed")]
        bool TlsSelfSignedCertificateAllowed { get; set; }

        /// <summary>
        /// Optional: The path to a trusted root certificate used to validate a TLS certificate received through the TLS handshaking process.
        /// </summary>
        /// <value>Paht to the TLS trusted certificate.</value>
        [Export("tlsTrustedCertificatePath")]
        NSUrl TlsTrustedCertificatePath { get; set; }

        /// <summary>
        /// Optional: Custom RTSP authentication logic, can be used if a provided RTSP stream is protected by a bespoke non-standard authentication method.
        /// </summary>
        /// <value>The custom authenticator.</value>
        [Export("authenticator")]
        SurgeAuthenticator Authenticator { get; set; }
    }

    /// <summary>
    /// Surge RTSP Delegate interface used to provide a notification of Surge library events.
    /// </summary>
    [BaseType(typeof(NSObject))]
    [Model]
    [Protocol]
    public interface SurgeRtspPlayerDelegate {
        /// <summary>
        /// Called once a stream has not received a frame from the RTSP stream within the defined timeout peroid as defined by SurgeRtspPlayer#Timeout.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method.</param>
        [Abstract]
        [Export("rtspPlayerDidTimeout:")]
        void RtspPlayerDidTimeout(SurgeRtspPlayer player);

        /// <summary>
        /// Called once stream playback has successfully started.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method.</param>
        [Abstract]
        [Export("rtspPlayerInitiatedPlayback:")]
        void RtspPlayerInitiatedPlayback(SurgeRtspPlayer player);

        /// <summary>
        /// Called if a stream has failed to successfully start playback.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method.</param>
        /// <param name="errorCode">RTSP error code causing playback to fail.</param>
        [Abstract]
        [Export("rtspPlayerFailedToInitiatePlayback:withErrorCode:")]
        void RtspPlayerFailedToInitiatePlayback(SurgeRtspPlayer player, RtspErrorCode errorCode);

        /// <summary>
        /// Called when the player begins or resumes playback of a stream.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method.</param>
        [Abstract]
        [Export("rtspPlayerDidBeginPlayback:")]
        void RtspPlayerDidBeginPlayback(SurgeRtspPlayer player);

        /// <summary>
        /// Called when the player stops or pauses playback of a stream.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method.</param>
        [Abstract]
        [Export("rtspPlayerDidStopPlayback:")]
        void RtspPlayerDidStopPlayback(SurgeRtspPlayer player);

        /// <summary>
        /// Called when the player enters the buffering state.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method.</param>
        [Abstract]
        [Export("rtspPlayerDidBeginBuffering:")]
        void RtspPlayerDidBeginBuffering(SurgeRtspPlayer player);

        /// <summary>
        /// Called when the player exits the buffering state.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method.</param>
        [Abstract]
        [Export("rtspPlayerDidStopBuffering:")]
        void RtspPlayerDidStopBuffering(SurgeRtspPlayer player);
    }

    /// <summary>
    /// Authenticator class used to inject custom authentication logic into Surge.
    /// </summary>
    [BaseType(typeof(NSObject))]
    [Model]
    [Protocol]
    public interface SurgeAuthenticator {

        /// <summary>
        /// Provides Surge with custom headers to append to each RTSP request.
        /// </summary>
        /// <returns>List of headers to append to each RTSP request.</returns>
        /// <param name="username">Username provided to authenticate the stream with.</param>
        /// <param name="password">Password provided to authenticate the stream with.</param>
        [Abstract]
        [Export("authenticationHeadersForUsername:andPassword:")]
        String[] AuthenticationHeaders(String username, String password);

        /// <summary>
        /// Provides a payload to be sent, encrypted (if applicable), down the pipe to authenticate the stream
        /// before Surge will attempt to execute any RTSP requests.
        /// </summary>
        /// <returns>payload to be sent down the pipe to authenticate the RTSP stream.</returns>
        /// <param name="username">Username provided to authenticate the stream with.</param>
        /// <param name="password">Password provided to authenticate the stream with.</param>
        [Abstract]
        [Export("firstBytesOnTheWireAuthenticationWithUsername:andPassword:")]
        NSData FirstBytesOnTheWireAuthentication(String username, String password);
    }

    /// <summary>
    /// 
    /// </summary>
    [BaseType(typeof(NSObject))]
    [Model]
    [Protocol]
    public interface SurgeDiagnosticsDelegate {

        /// <summary>
        /// Called every time the player calculates a new value for the number of frames arrived in the last second.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method</param>
        /// <param name="framerate">Current framerate of the playing video.</param>
        [Abstract]
        [Export("rtspPlayer:didObservePlaybackFrameRate:")]
        void RtspPlayerDidObservePlaybackFrameRate(SurgeRtspPlayer player, int framerate);

        /// <summary>
        /// Called every time the player calculates the bitrate of the video arriving from in the last second.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method</param>
        /// <param name="bitrate">Current bitrate of the playing video stream</param>
        [Abstract]
        [Export("rtspPlayer:didObservePlaybackBitrate:")]
        void RtspPlayerDidObservePlaybackBitrate(SurgeRtspPlayer player, int bitrate);

        /// <summary>
        /// Called once a second by the player with the latest information on the number of packets that have arrived as expected, have been lost or have arrived late.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method</param>
        /// <param name="packetLoss">Number of packets lost since the stream initially started playback.</param>
        /// <param name="successfulPacketsCount">Number of packets successfully receive as expected since the stream initially started playback.</param>
        /// <param name="reorderedPacketsCount">Number of packets recived out of order and since reordered since the stream initially started playback.</param>
        [Abstract]
        [Export("rtspPlayer:didObservePacketLossCount:withSuccessfulPacketsCount:andOutOfOrderPacketsReorderedCount:")]
        void RtspPlayerDidObservePacketLoss(SurgeRtspPlayer player, long packetLoss, long successfulPacketsCount, long reorderedPacketsCount);

        /// <summary>
        /// Called by the player any time the frame dimensions of the currently playing stream are changed.
        /// </summary>
        /// <param name="player">The Surge RTSP Player that called the delegate method</param>
        /// <param name="mediaSize">Dimensions of the video stream.</param>
        [Abstract]
        [Export("rtspPlayer:didChangeMediaSize:")]
        void RtspPlayerDidChangeMediaSize(SurgeRtspPlayer player, CGSize mediaSize);
    }

    /// <summary>
    /// Class containing the diagnostics details of the currently playing RTSP stream.
    /// </summary>
    [BaseType(typeof(NSObject))]
    [Model]
    [Protocol]
    public interface SurgeDiagnostics {
        /// <summary>
        /// Recorded FPS of the current stream.
        /// </summary>
        [Abstract]
        [Export("framesPerSecond")]
        int FramesPerSecond { get; }

        /// <summary>
        /// Recorded bitrate of the current stream.
        /// </summary>
        [Abstract]
        [Export("bitrate")]
        int Bitrate { get; }

        /// <summary>
        /// Number of packets which have arrived out of order and have had to be reordered before a frame was decoded in the lifetime of the currently playing stream.
        /// </summary>
        [Abstract]
        [Export("outOfOrderPacketsReceived")]
        long OutOfOrderPacketsReceived { get; }


        /// <summary>
        /// Number of packets which have successfully arrived in order as expected in the lifetime of the currently playing stream.
        /// </summary>
        [Abstract]
        [Export("successfulPacketsReceived")]
        long SuccessfulPacketsReceived { get; }

        /// <summary>
        /// Number of packets which have been lost/never arrived in the lifetime of the currently playing stream.
        /// </summary>
        [Abstract]
        [Export("lostPackets")]
        long LostPackets { get; }

        /// <summary>
        /// Video frame dimensions of the current stream.
        /// </summary>
        [Abstract]
        [Export("mediaDimensions")]
        CGSize MediaDimensions { get; }
    }

    public interface ISurgeDiagnostics { }

}
