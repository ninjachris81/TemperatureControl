package de.ninjachris.remotecontrol;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

/**
 * Created by B1 on 11.04.2015.
 */
public abstract class BTThread implements Runnable, OutputDataApi, InputDataApi {

    public static final int MESSAGE_READ = 44;

    private BluetoothSocket mmSocket;
    private BluetoothDevice device;

    private InputStream mmInStream;
    private OutputStream mmOutStream;
    private Activity activity;

    private final OutputDataApi outputDataApi;
    private final InputDataApi inputDataApi;

    private UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); //Standard SerialPortService ID

    protected abstract void onConnected(InputStream inStream, OutputStream outStream);

    public BTThread(final OutputDataApi outputDataApi, final InputDataApi inputDataApi, Activity activity) {
        this.outputDataApi = outputDataApi;
        this.inputDataApi = inputDataApi;
        this.activity = activity;
    }

    public void setDevice(BluetoothDevice device) {
        this.device = device;
    }

    @Override
    public void run() {
        try {
            // Get a BluetoothSocket to connect with the given BluetoothDevice
            mmSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
            mmSocket.connect();

            mmInStream = mmSocket.getInputStream();
            mmOutStream = mmSocket.getOutputStream();
            onConnected(mmInStream, mmOutStream);

            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    inputDataApi.onStateChanged(InputDataApi.STATE_CONNECTED);
                }
            });

            byte[] buffer = new byte[1024];  // buffer store for the stream
            int bytes; // bytes returned from read()

            StringBuffer sb = new StringBuffer();

            // Keep listening to the InputStream until an exception occurs
            while (true) {
                try {
                    // Read from the InputStream
                    bytes = mmInStream.read(buffer);
                    // Send the obtained bytes to the UI activity

                    if (bytes>0) {
                        sb.append(new String(buffer, 0, bytes));
                    }

                    if (sb.length()>0) {
                        int i = sb.indexOf(";");

                        while (i!=-1) {
                            String[] tokens = sb.toString().split(";", 2);
                            final String[] cmds = tokens[0].split(" ", 2);

                            activity.runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Log.d("BTThread", "onDataReceived: " + cmds[0] + cmds[1]);
                                    inputDataApi.onDataReceived(cmds[0], cmds[1]);
                                }
                            });

                            sb.setLength(0);
                            sb.append(tokens[1]);
                            i = sb.indexOf(";");
                        }
                    }

                } catch (IOException e) {
                    break;
                }
            }

        } catch (IOException connectException) {
            // Unable to connect; close the socket and get out
            inputDataApi.onStateChanged(InputDataApi.STATE_DISCONNECTED);

            try {
                mmSocket.close();
            } catch (IOException closeException) { }
            return;
        }
    }

    private void sendData(String data) {
        if (mmOutStream!=null) {
            try {
                mmOutStream.write(data.getBytes());
                mmOutStream.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void sendData(String receiver, String cmd) {
        sendData(receiver + " " + cmd + ";");
    }

    /** Will cancel an in-progress connection, and close the socket */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }

}
