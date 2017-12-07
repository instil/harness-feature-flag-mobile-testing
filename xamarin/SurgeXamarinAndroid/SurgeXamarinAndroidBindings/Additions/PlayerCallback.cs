using System;
using System.Runtime.InteropServices;

public class PlayerCallback: Java.Lang.Object, CO.Instil.Surge.Callbacks.IPlayerCallback {

    private Action<int> callback;

    public PlayerCallback(Action<int> callback) {
        this.callback = callback;
    }

    public void Response(int errorCode) {
        if (callback != null) {
            callback(errorCode);
        }
    }
}
