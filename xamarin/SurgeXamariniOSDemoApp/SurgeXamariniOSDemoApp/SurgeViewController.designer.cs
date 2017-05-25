// WARNING
//
// This file has been generated automatically by Visual Studio from the outlets and
// actions declared in your storyboard file.
// Manual changes to this file will not be maintained.
//
using Foundation;
using System;
using System.CodeDom.Compiler;
using UIKit;

namespace SurgeXamariniOSDemoApp
{
    [Register ("SurgeViewController")]
    partial class SurgeViewController
    {
        [Outlet]
        [GeneratedCode ("iOS Designer", "1.0")]
        UIKit.UIToolbar ControlsToolbar { get; set; }

        [Outlet]
        [GeneratedCode ("iOS Designer", "1.0")]
        UIKit.UILabel FpsLabel { get; set; }

        [Outlet]
        [GeneratedCode ("iOS Designer", "1.0")]
        UIKit.UIBarButtonItem PauseButton { get; set; }

        [Outlet]
        [GeneratedCode ("iOS Designer", "1.0")]
        UIKit.UIBarButtonItem PlayButton { get; set; }

        [Outlet]
        [GeneratedCode ("iOS Designer", "1.0")]
        UIKit.UIButton StartStreamButton { get; set; }

        [Outlet]
        [GeneratedCode ("iOS Designer", "1.0")]
        UIKit.UILabel StatusLabel { get; set; }

        [Outlet]
        [GeneratedCode ("iOS Designer", "1.0")]
        UIKit.UITextField UrlBar { get; set; }

        [Outlet]
        [GeneratedCode ("iOS Designer", "1.0")]
        UIKit.UIImageView VideoImageView { get; set; }

        [Action ("PauseButton_Activated:")]
        [GeneratedCode ("iOS Designer", "1.0")]
        partial void PauseButton_Activated (UIKit.UIBarButtonItem sender);

        [Action ("PlayButton_Activated:")]
        [GeneratedCode ("iOS Designer", "1.0")]
        partial void PlayButton_Activated (UIKit.UIBarButtonItem sender);

        [Action ("StartStreamButton_TouchUpInside:")]
        [GeneratedCode ("iOS Designer", "1.0")]
        partial void StartStreamButton_TouchUpInside (UIKit.UIButton sender);

        void ReleaseDesignerOutlets ()
        {
            if (ControlsToolbar != null) {
                ControlsToolbar.Dispose ();
                ControlsToolbar = null;
            }

            if (FpsLabel != null) {
                FpsLabel.Dispose ();
                FpsLabel = null;
            }

            if (PauseButton != null) {
                PauseButton.Dispose ();
                PauseButton = null;
            }

            if (PlayButton != null) {
                PlayButton.Dispose ();
                PlayButton = null;
            }

            if (StartStreamButton != null) {
                StartStreamButton.Dispose ();
                StartStreamButton = null;
            }

            if (StatusLabel != null) {
                StatusLabel.Dispose ();
                StatusLabel = null;
            }

            if (UrlBar != null) {
                UrlBar.Dispose ();
                UrlBar = null;
            }

            if (VideoImageView != null) {
                VideoImageView.Dispose ();
                VideoImageView = null;
            }
        }
    }
}