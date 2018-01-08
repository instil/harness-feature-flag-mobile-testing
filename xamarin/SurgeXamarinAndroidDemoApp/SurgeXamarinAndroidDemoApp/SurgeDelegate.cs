using System;
using CO.Instil.Surge.Player;

namespace SurgeXamarinAndroidDemoApp {
    public class SurgeDelegate : Java.Lang.Object, ISurgeRtspPlayerDelegate {
        public SurgeDelegate() { }

        public void RtspPlayerDidTimeout() {
            Console.WriteLine("Surge: Stream has timed out.");
        }

        public void RtspPlayerDidUpdateFps(int fps) {
            Console.WriteLine("FPS: {0}", fps);
        }
    }
}
