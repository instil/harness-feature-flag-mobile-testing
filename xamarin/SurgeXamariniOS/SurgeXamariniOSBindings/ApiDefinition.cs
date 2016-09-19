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
        void InitiatePlaybackOf(NSUrl url);

        [Export("stop")]
        void Stop();

        [Export("pause")]
        void Pause();

        [Export("play")]
        void Play();

        [Export("playerView")]
        UIImageView PlayerView { get; set; }
    }
}

