using System;
using ObjCRuntime;
using UIKit;
using Foundation;
using SurgeXamariniOSBindings;

namespace SurgeiOSTestApp
{
	public partial class ViewController : UIViewController
    {
        SurgeRtspPlayer test;

        protected ViewController(IntPtr handle) : base(handle) { }

        public override void ViewDidLoad()
		{
			base.ViewDidLoad();
			// Perform any additional setup after loading the view, typically from a nib.

			test = new SurgeRtspPlayer();
			test.PlayerView = VideoImageView;
            //test.InterleavedRtspTransport = true;
			test.InitiatePlaybackOf(NSUrl.FromString("rtsp://192.168.1.201:8554/test"));
		}

		public override void DidReceiveMemoryWarning()
		{
			base.DidReceiveMemoryWarning();
			// Release any cached data, images, etc that aren't in use.
		}
	}
}

