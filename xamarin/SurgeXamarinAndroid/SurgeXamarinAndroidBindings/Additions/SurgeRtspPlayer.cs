using System;
using System.Runtime.InteropServices;
using Android.Runtime;
using Android.Views;
using Java.Util;
using CO.Instil.Surge.Client;

namespace CO.Instil.Surge.Player {
    public partial class SurgeRtspPlayer {

        public PlayerCallback playerCallback;

        public void InitiatePlaybackOf(string url, SurgeSurface surface, Action<int> callback)
        {
            InitiatePlaybackOf(url, surface, "", "", null, null, callback);
        }

        public void InitiatePlaybackOf(string url, SurgeSurface surface, string username, string password, Action<int> callback)
        {
            InitiatePlaybackOf(url, surface, username, password, null, null, callback);
        }

        public void InitiatePlaybackOf(string url, SurgeSurface surface, string username, string password, Date startTime, Date endTime, Action<int> callback)
        {
            playerCallback = new PlayerCallback(callback);
            InitiatePlaybackOf(url, surface, username, password, startTime, endTime, playerCallback);
        }
    }
}
