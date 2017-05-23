using Android.App;
using Android.Widget;
using Android.OS;
using Android.Views;
using Android.Graphics;
using System;
using CO.Instil.Surge.Player;
using CO.Instil.Surge.Callbacks;
using CO.Instil.Surge.Client;

namespace SurgeXamarinAndroid
{
	[Activity(Label = "SurgeXamarinAndroid", MainLauncher = true, Icon = "@mipmap/icon")]
	public class MainActivity : Activity, TextureView.ISurfaceTextureListener
	{
		RtspPlayer player;
		private TextureView textureView;

		protected override void OnCreate(Bundle savedInstanceState)
		{
			base.OnCreate(savedInstanceState);

			player = new RtspPlayer();

			SetContentView(Resource.Layout.activity_main);
			textureView = (TextureView)FindViewById(Resource.Id.textureView);
			textureView.SurfaceTextureListener = this;

			//// Set our view from the "main" layout resource
			//SetContentView(Resource.Layout.Main);

			//
			// Get our button from the layout resource,
			//// and attach an event to it
			//Button button = FindViewById<Button>(Resource.Id.

			//button.Click += delegate { button.Text = string.Format("{0} clicks!", count++); };
		}

		public void OnSurfaceTextureAvailable(SurfaceTexture surface, int width, int height)
		{
            player.InitiatePlaybackOf("rtsp://localhost:8554/test", new SurgeSurface(surface, width, height), (bool result) => {
                Console.WriteLine(result);
            });
   		}

		public bool OnSurfaceTextureDestroyed(SurfaceTexture surface)
		{
			player.Stop();
            return false;
		}

		public void OnSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height)
		{
			//throw new NotImplementedException();
		}

		public void OnSurfaceTextureUpdated(SurfaceTexture surface)
		{
			//throw new NotImplementedException();
		}
	}
}


