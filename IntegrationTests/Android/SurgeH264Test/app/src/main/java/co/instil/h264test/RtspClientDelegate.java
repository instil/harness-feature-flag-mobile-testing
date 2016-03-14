package co.instil.h264test;

/**
 * Created by redbrain on 10/03/2016.
 */
public interface RtspClientDelegate {

    void Timeout();

    void Payload(byte[] payload);

}
