using System;
using SurgeXamariniOSBindings;
using UIKit;

namespace SurgeXamariniOSDemoApp {
    public class SurgeDelegate: SurgeRtspPlayerDelegate {
        private static readonly string fpsMessage = "FPS: {0}";

        private UILabel statusLabel;
        private UILabel fpsLabel;

		public SurgeDelegate(UILabel statusLabel, UILabel fpsLabel) {
			this.statusLabel = statusLabel;
			this.fpsLabel = fpsLabel;
		}

        public override void RtspPlayerDidBeginBuffering(SurgeRtspPlayer player) {
			InvokeOnMainThread(() => {
				statusLabel.Text = "Stream has started buffering";
			});
        }

        public override void RtspPlayerDidBeginPlayback(SurgeRtspPlayer player) {
			InvokeOnMainThread(() => {
				statusLabel.Text = "Stream playback has started";
			});
        }

        public override void RtspPlayerDidObservePlaybackFramerate(SurgeRtspPlayer player, int framerate) {
			InvokeOnMainThread(() => {
                fpsLabel.Text = String.Format(fpsMessage, framerate);
			});
        }

        public override void RtspPlayerDidStopBuffering(SurgeRtspPlayer player) {
			InvokeOnMainThread(() => {
				statusLabel.Text = "Stream has stopped buffering";
			});
        }

        public override void RtspPlayerDidStopPlayback(SurgeRtspPlayer player) {
			InvokeOnMainThread(() => {
				statusLabel.Text = "Stream has stopped playback";
			});
        }

        public override void RtspPlayerDidTimeout(SurgeRtspPlayer player) {
			InvokeOnMainThread(() => {
				statusLabel.Text = "RTSP Player has timed out";
			});
        }

        public override void RtspPlayerFailedToInitiatePlayback(SurgeRtspPlayer player) {
			InvokeOnMainThread(() => {
				statusLabel.Text = "Failed to initiate playback for URL <URL>";
			});
		}

        public override void RtspPlayerInitiatedPlayback(SurgeRtspPlayer player) {
			InvokeOnMainThread(() => {
				statusLabel.Text = "Playback has started for URL <URL>";
			});
		}
    }
}
