/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

using Foundation;
using System;
using UIKit;
using SurgeXamariniOSBindings;

namespace SurgeXamariniOSDemoApp
{
    public partial class SurgeViewController : UIViewController {

		private SurgeRtspPlayer player;
        private SurgeDelegate surgeDelegate;

        public SurgeViewController (IntPtr handle) : base (handle)  { }

		public override void ViewDidLoad() {
			base.ViewDidLoad();

            player = new SurgeRtspPlayer();
			player.PlayerView = VideoImageView;

            surgeDelegate = new SurgeDelegate(StatusLabel, FpsLabel);
            player.ClientDelegate = surgeDelegate;

            PlayButtonEnabled();

            UrlBar.Text = GetStoredUrl();
            UrlBar.ShouldReturn += (textField) => {
				textField.ResignFirstResponder();

				PlayStream(textField.Text);

				return true;
            };

		}

        partial void StartStreamButton_TouchUpInside(UIButton sender) {
            PlayStream(UrlBar.Text);
			this.View.EndEditing(true);
        }

        partial void PauseButton_Activated(UIBarButtonItem sender) {
            player.Pause();
			PlayButtonEnabled();
        }

        partial void PlayButton_Activated(UIBarButtonItem sender) {
			player.Play();
			PauseButtonEnabled();
		}

        private void PlayStream(String url) {
			player.InitiatePlaybackOf(NSUrl.FromString(url));
			PauseButtonEnabled();
			SaveUrl(url);
        }

		private void PlayButtonEnabled() {
			PauseButton.Enabled = false;
			PlayButton.Enabled = true;
		}

        private void PauseButtonEnabled() {
			PauseButton.Enabled = true;
            PlayButton.Enabled = false;
		}

		private void AllButtonsDisabled() {
            PauseButton.Enabled = false;
			PlayButton.Enabled = false;
		}

		private void SaveUrl(string url) {
			NSUserDefaults.StandardUserDefaults.SetString(url, "url");
		}

		private string GetStoredUrl() {
            return NSUserDefaults.StandardUserDefaults.StringForKey("url");
		}
    }
}