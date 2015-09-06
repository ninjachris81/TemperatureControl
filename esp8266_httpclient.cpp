#include "esp8266_httpclient.h";
#include "http.h"

void ESP8266HttpClient::init(HttpClientTaskCallbackHandler* tcbh) {
  this->tcbh = tcbh;
  myEsp.setTaskCallbackHandler(this);
}

void ESP8266HttpClient::initConnection(String apName, String password) {
  this->apName = apName;
  this->password = password;
  myEsp.init();
  myEsp.softReset();
}

void ESP8266HttpClient::deinitConnection() {
  myEsp.apQuit();
}


void ESP8266HttpClient::update() {
  myEsp.update();
}

void ESP8266HttpClient::connectToServer(String ip, int port, bool tryReconnect) {
  this->ip = ip;
  this->port = port;
  this->tryReconnect = tryReconnect;
  myEsp.connectToServer(CONN_ID, TCP_MODE, ip, port);
}

void ESP8266HttpClient::executeGET(String url, String host, String connMode) {
  executeRequest(HTTP_METHOD_GET, url, host, connMode, HTTP_CONTENT_TYPE_TEXT_HTML);
}

void ESP8266HttpClient::executeHEAD(String url, String host, String connMode) {
  executeRequest(HTTP_METHOD_HEAD, url, host, connMode, "");
}

void ESP8266HttpClient::executeRequest(String method, String url, String host, String connMode, String accept) {
  String data = "";
  data+=method;
  data+=" ";
  data+=url;
  data+=F(" HTTP/1.1\r\nConnection: ");
  data+=connMode;
  data+=F("\r\nHost: ");
  data+=host;
  data+=F("\r\n");
  if (accept.length()>0) {
    data+=F("Accept: ");
    data+=accept;
    data+=F("\r\n");
  }
  data+=F("\r\n");

  myEsp.sendData(CONN_ID, data);
}


void ESP8266HttpClient::disconnectFromServer() {
  myEsp.closeConn(CONN_ID);
}

void ESP8266HttpClient::softResetComplete(bool success) {
  if (success) {
    myEsp.echoOff();
  } else {
    // actually, a hard reset required not
    myEsp.softReset();
  }
}

void ESP8266HttpClient::echoOffComplete(bool success) {
  if (success) {
    myEsp.test();
  } else {
    myEsp.softReset();
  }
}

void ESP8266HttpClient::testComplete(bool success) {
  if (success) {
    myEsp.setNetworkMode(NETWORK_MODE_STA);
  } else {
    myEsp.softReset();
  }
}

void ESP8266HttpClient::networkModeComplete(bool success) {
  if (success) {
    myEsp.apJoin(apName, password);
  } else {
    myEsp.softReset();
  }
}

void ESP8266HttpClient::apJoinComplete(bool success) {
  if (success) {
  myEsp.setConnMode(CONN_MODE_SINGLE);
  } else {
    myEsp.apJoin(apName, password);
  }
}

void ESP8266HttpClient::connModeComplete(bool success) {
  tcbh->initComplete(success);
}

void ESP8266HttpClient::connectToServerComplete(bool success) {
  if (success) {
    tcbh->connectToServerComplete(success);
  } else {
    if (tryReconnect) {
      myEsp.connectToServer(CONN_ID, TCP_MODE, this->ip, this->port);
    }
  }
}

void ESP8266HttpClient::sendDataComplete(bool success) {
  if (success) {
    String response = myEsp.readResponse();
    tcbh->executeGETComplete(response.length()>0, response);
  } else {
    tcbh->executeGETComplete(false, "");
  }
}

void ESP8266HttpClient::closeConnComplete(bool success) {
  tcbh->disconnectComplete(success);  
}

void ESP8266HttpClient::apQuitComplete(bool success) {
  tcbh->deinitComplete(success);
}

