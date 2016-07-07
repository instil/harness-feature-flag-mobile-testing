package co.instil.surge.client;

/**
 *
 */
public class RtspClient implements AutoCloseable {

    static {
        System.loadLibrary("surge");
        load();
    }

    private final long nativeClient;

    public RtspClient(RtspClientDelegate delegate) {
        nativeClient = createNativeClientInstance(delegate);
    }

    private static native void load();

    private native long createNativeClientInstance(RtspClientDelegate delegate);

    public native Response options();

    public native Response options(String url);

    public native DescribeResponse describe(String url);

    public native DescribeResponse describe(String url, String username, String password);

    public native Response setup(SessionDescription sessionDescription);

    public native Response play();

    public native Response pause();

    public native Response tearDown();

    public native Response keepAlive();

    public native void close();

}
