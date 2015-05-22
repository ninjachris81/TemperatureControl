package de.ninjachris.remotecontrol;

/**
 * Created by B1 on 22.05.2015.
 */
public interface InputDataApi {

    public static final int STATE_DISCONNECTED = 0;
    public static final int STATE_CONNECTED = 1;

    public void onDataReceived(String receiver, String cmd);

    public void onStateChanged(int state);
}
