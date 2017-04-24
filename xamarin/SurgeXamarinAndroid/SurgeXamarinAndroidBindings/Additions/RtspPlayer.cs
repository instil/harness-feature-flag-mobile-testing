using System;
using System.Runtime.InteropServices;
using Android.Runtime;
using Android.Views;
using Java.Util;

namespace CO.Instil.Surge.Player {
    public partial class RtspPlayer {

        public PlayerCallback playerCallback;

        public void InitiatePlaybackOf(string url, Surface surface, Action<bool> callback)
        {
            InitiatePlaybackOf(url, surface, "", "", null, null, callback);
        }

        public void InitiatePlaybackOf(string url, Surface surface, string username, string password, Action<bool> callback)
        {
            InitiatePlaybackOf(url, surface, username, password, null, null, callback);
        }

        public void InitiatePlaybackOf(string url, Surface surface, string username, string password, Date startTime, Date endTime, Action<bool> callback)
        {
            playerCallback = new PlayerCallback(callback);
            InitiatePlaybackOf(url, surface, username, password, startTime, endTime, playerCallback);
        }
    }
}
