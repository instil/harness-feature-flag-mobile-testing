using System;
using System.Runtime.InteropServices;

public class PlayerCallback: Java.Lang.Object, CO.Instil.Surge.Callbacks.IPlayerCallback {

    private Action<bool> callback;

    public PlayerCallback(Action<bool> callback) {
        this.callback = callback;
    }

    public void Response(bool result) {
        if (callback != null) {
            callback(result);
        }
    }
}
