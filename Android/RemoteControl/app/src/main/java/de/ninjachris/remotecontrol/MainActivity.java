package de.ninjachris.remotecontrol;

import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.ConsoleMessage;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;


public class MainActivity extends ActionBarActivity {

    private static final int REQUEST_ENABLE_BT = 1;

    private static final String BT_DEVICE_NAME = "TEMP_CTRL";

    private BluetoothAdapter mBluetoothAdapter;

    private WebView webView;

    private DuplexDataJsImpl dataApi;
    private OutputDataApi outputDataApi;
    private BTThread btThread;

    private static final String AUTH_TOKEN = "14624";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        webView = (WebView) findViewById(R.id.webView);
        dataApi = new DuplexDataJsImpl(webView);

        btThread = new BTThread(dataApi, dataApi, this) {
            @Override
            protected void onConnected(InputStream inStream, OutputStream outStream) {
                sendData("BT", "AUTH " + AUTH_TOKEN);
            }

            @Override
            public void onDataReceived(String receiver, String cmd) {
            }

            @Override
            public void onStateChanged(int state) {
            }
        };
        dataApi.setOutputDataApi(btThread);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            // Device does not support Bluetooth
            Toast.makeText(this, "BT not supported !", Toast.LENGTH_LONG).show();
        } else {
            if (!mBluetoothAdapter.isEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            } else {
                tryConnect(BT_DEVICE_NAME);
            }
        }

        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);

        webView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onConsoleMessage(ConsoleMessage consoleMessage) {
                Log.d("WebView", consoleMessage.message() + " -- From line "
                        + consoleMessage.lineNumber() + " of "
                        + consoleMessage.sourceId());
                return true;
            }
        });

        webView.loadUrl("file:///android_asset/html/index.html");
        webView.addJavascriptInterface(dataApi, "dataApi");
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode==REQUEST_ENABLE_BT && resultCode==RESULT_OK) {
            // BT activated
            tryConnect(BT_DEVICE_NAME);
        }
    }

    private void tryConnect(final String deviceName) {
        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();

        for (BluetoothDevice device : pairedDevices) {
            if (device.getName().equals(deviceName)) {
                tryConnect(device);
            }
        }
    }

    private void tryConnect(BluetoothDevice device) {
        btThread.setDevice(device);
        new Thread(btThread).start();
    }
}
