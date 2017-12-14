using System;
using System.Runtime.InteropServices;

public class PlayerCallback: Java.Lang.Object, CO.Instil.Surge.Callbacks.IPlayerCallback {

    private Action<CO.Instil.Surge.Player.RtspErrorCode> callback;

    public PlayerCallback(Action<CO.Instil.Surge.Player.RtspErrorCode> callback) {
        this.callback = callback;
    }

    public void Response(CO.Instil.Surge.Player.RtspErrorCode errorCode) {
        if (callback != null) {
            callback(errorCode);
        }
    }
}
