using System;
using Java.Util;
using CO.Instil.Surge.Client;

namespace CO.Instil.Surge.Player {
    public partial class SurgeRtspPlayer {

        public PlayerCallback playerCallback;

        public void InitiatePlaybackOf(string url, Action<RtspErrorCode> callback) {
            InitiatePlaybackOf(url, "", "", null, null, callback);
        }

        public void InitiatePlaybackOf(string url, string username, string password, Action<RtspErrorCode> callback) {
            InitiatePlaybackOf(url, username, password, null, null, callback);
        }

        public void InitiatePlaybackOf(string url, string username, string password, Date startTime, Date endTime, Action<RtspErrorCode> callback) {
            playerCallback = new PlayerCallback(callback);
            InitiatePlaybackOf(url, username, password, startTime, endTime, playerCallback);
        }

        public void Seek(Date startTime, Date endTime, Action<RtspErrorCode> callback) {
            Seek(startTime, endTime, new PlayerCallback(callback));
        }
    }
}
