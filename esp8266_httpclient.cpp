#include "esp8266_httpclient.h";
#include "http.h"

void ESP8266HttpClient::init(ESP8266 *myEsp) {
  this->myEsp = myEsp;

  if (!myEsp->setConnMode(CONN_MODE_SINGLE)) return;
}

void ESP8266HttpClient::update() {
}

bool ESP8266HttpClient::connectToServer(String ip, int port) {
  if (!myEsp->clientMode(CONN_ID, TCP_MODE, ip, port)) {
    return myEsp->clientMode(CONN_ID, TCP_MODE, ip, port);    // try again
  }
}

bool ESP8266HttpClient::executeGET(String url, String host, String connMode, String &response) {
  return executeRequest(HTTP_METHOD_GET, url, host, connMode, HTTP_CONTENT_TYPE_TEXT_HTML, response);
}

bool ESP8266HttpClient::executeHEAD(String url, String host, String connMode, String &response) {
  return executeRequest(HTTP_METHOD_HEAD, url, host, connMode, "", response);
}

bool ESP8266HttpClient::executeRequest(String method, String url, String host, String connMode, String accept, String &response) {
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

  if (myEsp->sendData(CONN_ID, data)) {
    response = myEsp->readResponse();
    return response.length()>0;
  } else {
    return false;
  }
}


bool ESP8266HttpClient::disconnectFromServer() {
  return (myEsp->connClose(CONN_ID));
}

