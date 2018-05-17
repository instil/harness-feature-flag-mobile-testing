using Android.App;
using Android.Widget;
using Android.OS;
using Android.Views;
using Android.Graphics;
using CO.Instil.Surge.Player;
using System;
using CO.Instil.Surge.Client;
using Android.Content;
using Android.Preferences;

namespace SurgeXamarinAndroidDemoApp {
    [Activity(Label = "Surge Xamarin Android Demo App", MainLauncher = true, Icon = "@mipmap/icon")]
    public class MainActivity : Activity, TextureView.ISurfaceTextureListener {

        SurgeRtspPlayer player;
        SurgeDelegate surgeDelegate;
        TextureView textureView;

        protected override void OnCreate(Bundle savedInstanceState) {
            base.OnCreate(savedInstanceState);

            SetupView();

            surgeDelegate = new SurgeDelegate();
            player = new SurgeRtspPlayer();
            player.Delegate = surgeDelegate;
        }

        public void SetupView() {
            SetContentView(Resource.Layout.Main);

            textureView = FindViewById<TextureView>(Resource.Id.textureView);
            textureView.SurfaceTextureListener = this;

            var button = FindViewById<Button>(Resource.Id.startStreamButton);
            var urlBar = FindViewById<EditText>(Resource.Id.urlBar);

            button.Click += delegate {
                StartPlayback(urlBar.Text);
                SaveUrl(urlBar.Text);
            };

            urlBar.Text = GetStoredUrl();
        }

        public void StartPlayback(String url) {
            player.InitiatePlaybackOf(url, null, (RtspErrorCode errorCode) => {
                Console.WriteLine(errorCode);
            });
        }

        public void OnSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            player.SetSurface(new SurgeSurface(surface, width, height));
        }

        public bool OnSurfaceTextureDestroyed(SurfaceTexture surface) {
            player.Stop();
            return false;

        }

        public void OnSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) { }

        public void OnSurfaceTextureUpdated(SurfaceTexture surface) { }

        public string GetStoredUrl() {
            var prefs = PreferenceManager.GetDefaultSharedPreferences(this);
            return prefs.GetString("lastUsedUrl", "");
        }

        public void SaveUrl(String url) {
            var prefs = PreferenceManager.GetDefaultSharedPreferences(this);
            var editor = prefs.Edit();
            editor.PutString("lastUsedUrl", url);
            editor.Apply();
        }
    }
}

