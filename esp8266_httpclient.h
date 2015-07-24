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

class ESP8266HttpClient {
public:
  void init(ESP8266 *myEsp);
  
  void update();
  
  bool connectToServer(String ip, int port = 80);
  
  bool executeGET(String url, String host, String connMode, String &response);
  
  bool executeHEAD(String url, String host, String connMode, String &response);

  bool disconnectFromServer();
  
private:
  ESP8266 *myEsp;

  bool executeRequest(String method, String url, String host, String connMode, String accept, String &response);

};


#endif
