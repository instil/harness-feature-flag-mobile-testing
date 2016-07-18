using System;
using ObjCRuntime;
using UIKit;
using SurgeiOSBindings;
using Foundation;

namespace SurgeiOSTestApp
{
	public partial class ViewController : UIViewController
	{
		//public static UIStoryboard Storyboard = UIStoryboard.FromName("Main", null);

		protected ViewController(IntPtr handle) : base(handle)
		{
			// Note: this .ctor should not contain any initialization logic.
		}

		public override void ViewDidLoad()
		{
			base.ViewDidLoad();
			// Perform any additional setup after loading the view, typically from a nib.

			SurgeRtspPlayer test = new SurgeRtspPlayer();
			test.PlayerView = VideoImageView;
			test.InitiatePlaybackOf(NSUrl.FromString("rtsp://192.168.1.24:8554/test"));
		}

		public override void DidReceiveMemoryWarning()
		{
			base.DidReceiveMemoryWarning();
			// Release any cached data, images, etc that aren't in use.
		}
	}
}

