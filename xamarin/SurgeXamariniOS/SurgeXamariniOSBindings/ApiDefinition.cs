using System;

using UIKit;
using Foundation;
using ObjCRuntime;
using CoreGraphics;

namespace SurgeXamariniOSBindings
{
    [BaseType(typeof(NSObject))]
    public interface SurgeRtspPlayer
    {
        [Export("initiatePlaybackOf:")]
        bool InitiatePlaybackOf(NSUrl url);

        [Export("stop")]
        void Stop();

        [Export("pause")]
        void Pause();

        [Export("play")]
        void Play();

        [Export("playerView")]
        UIImageView PlayerView { get; set; }

        [Export("delegate")]
        SurgeRtspPlayerDelegate ClientDelegate { get; set; }
    }

    [BaseType(typeof(NSObject))]
    [Model][Protocol]
    public interface SurgeRtspPlayerDelegate
    {
        [Abstract][Export("rtspPlayerDidTimeout:")]
        void RtspPlayerDidTimeout(SurgeRtspPlayer player);

        [Abstract][Export("rtspPlayerInitiatedPlayback:")]
        void RtspPlayerInitiatedPlayback(SurgeRtspPlayer player);

        [Abstract][Export("rtspPlayerFailedToInitiatePlayback:")]
        void RtspPlayerFailedToInitiatePlayback(SurgeRtspPlayer player);
    }
}

