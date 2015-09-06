#ifndef esp8266_httpclient_h
#define esp8266_httpclient_h

#include <SoftwareSerial.h>
#include "esp8266.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define HTTP_CONN_MODE_CLOSE F("close")
#define HTTP_CONN_MODE_KEEP_ALIVE F("keep-alive")

#define HTTP_METHOD_GET F("GET")
#define HTTP_METHOD_HEAD F("HEAD")

#define CONN_ID -1

class ESP8266HttpClient : public ESP8266::TaskCallbackHandler {
public:
  class HttpClientTaskCallbackHandler {
    public:
    virtual void initComplete(bool success) = 0;
    virtual void connectToServerComplete(bool success) = 0;
    virtual void executeGETComplete(bool success, String response) = 0;
    virtual void disconnectComplete(bool success) = 0;
    virtual void deinitComplete(bool success) = 0;
  };

  void init(HttpClientTaskCallbackHandler* tcbh);

  void initConnection(String apName, String password);

  void deinitConnection();
  
  void update();
  
  void connectToServer(String ip, int port = 80, bool tryReconnect = true);
  
  void executeGET(String url, String host, String connMode);
  
  void executeHEAD(String url, String host, String connMode);

  void disconnectFromServer();
  
private:
  ESP8266 myEsp;
  HttpClientTaskCallbackHandler *tcbh;

  String ip;
  int port;

  String apName;
  String password;
  bool tryReconnect;

  void executeRequest(String method, String url, String host, String connMode, String accept);

  void softResetComplete(bool success);
  void echoOffComplete(bool success);
  void testComplete(bool success);
  void networkModeComplete(bool success);
  void apJoinComplete(bool success);
  void connModeComplete(bool success);
  void connectToServerComplete(bool success);
  void sendDataComplete(bool success);
  void closeConnComplete(bool success);
  void apQuitComplete(bool success);
  
};


#endif
