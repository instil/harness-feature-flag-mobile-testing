package co.instil.surge.client;

/**
 *
 */
public class RtspClient implements AutoCloseable {

    static {
        System.loadLibrary("surge");
    }

    private final long nativeClient;

    public RtspClient(RtspClientDelegate delegate) {
        nativeClient = createNativeClientInstance(delegate);
    }

    private native long createNativeClientInstance(RtspClientDelegate delegate);

    public native Response options();

    public native Response options(String url);

    public native DescribeResponse describe(String url);

    public native DescribeResponse describe(String url, String username, String password);

    public native SetupResponse setup(SessionDescription sessionDescription);

    public native Response play();

    public native Response pause();

    public native Response tearDown();

    public native Response keepAlive();

    public native void close();

}
