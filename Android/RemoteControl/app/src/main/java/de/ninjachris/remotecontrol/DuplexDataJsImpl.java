package de.ninjachris.remotecontrol;

import android.webkit.JavascriptInterface;
import android.webkit.WebView;

/**
 * Created by B1 on 16.05.2015.
 */
public class DuplexDataJsImpl implements OutputDataApi, InputDataApi {

    private WebView webView;
    private OutputDataApi outputDataApi;

    public DuplexDataJsImpl(WebView webView) {
        this.webView = webView;
    }

    public void setOutputDataApi(OutputDataApi outputDataApi) {
        this.outputDataApi = outputDataApi;
    }

    @Override
    @JavascriptInterface
    public void sendData(String receiver, String cmd) {
        outputDataApi.sendData(receiver, cmd);
    }

    @Override
    public void onStateChanged(int state) {
        webView.evaluateJavascript("onStateChanged(" + state + ")", null);
    }

    @Override
    public void onDataReceived(String receiver, String cmd) {
        webView.evaluateJavascript("onDataReceived('" + receiver + "','" + cmd + "')", null);
    }
}
