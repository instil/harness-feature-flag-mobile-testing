using System;

using UIKit;
using Foundation;
using ObjCRuntime;
using CoreGraphics;

namespace SurgeXamariniOSBindings
{
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
    public interface SurgeRtspPlayer
    {
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
        /// Once Stop has been requested, the stream canno tbe restarted via the Play() command. To restart a stream, plese re-execute the InitiatePlaybackOf() command
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
		int Timeout 
        { get; set; }

		/// <summary>
		/// If true, Surge will stream video data via an interleaved TCP transport rather than via UDP
		/// </summary>
		[Export("interleavedTcpTransport")]
		bool InterleavedTcpTransport { get; set; }

	}

    /// <summary>
    /// Surge RTSP Delegate interface used to provide a notification of Surge library events.
    /// </summary>
    [BaseType(typeof(NSObject))]
    [Model][Protocol]
    public interface SurgeRtspPlayerDelegate
    {
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
		[Abstract]
        [Export("rtspPlayerFailedToInitiatePlayback:")]
		void RtspPlayerFailedToInitiatePlayback(SurgeRtspPlayer player);

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

		/// <summary>
		/// Guaranteed to be called at most once per second with the current player frame rate.
		/// </summary>
		/// <param name="player">The Surge RTSP Player that called the delegate method.</param>
		[Abstract]
		[Export("rtspPlayer:didObservePlaybackFrameRate:")]
		void RtspPlayerDidObservePlaybackFramerate(SurgeRtspPlayer player, int framerate);
    }
}

