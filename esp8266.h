#ifndef esp8266_h
#define esp8266_h

#include <SoftwareSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "globals.h"

#define ESP8266_MODULE_NAME MODNAME("ESP")

#define ESP_BAUD_RATE 115200
//#define ESP_BAUD_RATE 921600
//#define ESP_BAUD_RATE 9600

#define CONN_MODE_SINGLE 0
#define CONN_MODE_MULTI 1

#define SERVER_MODE_CLOSE 0
#define SERVER_MODE_OPEN 1

#define TCP_MODE F("TCP")
#define UDP_MODE F("UDP")

#define SEND_BUFFER_SIZE 64

class ESP8266 {
public:
  
  void init();
  
  void update();
  
  void setBaudRate();
  
  bool setTimeout(int timeoutMs);
  
  bool test();
  
  bool softReset();
  
  bool echoOff();
  
  void getStatus();
  
  bool setConnMode(int mode);
  
  bool serverMode(int mode, int port = -1);
  
  bool clientMode(int id, String type, String ip, int port);
  
  bool sendData(int id, String data);

  bool beginData(int id, uint16_t dataLength);

  bool postData(byte data);
  
  bool apInRange(String name);
  
  bool apJoin(String name, String password);
  
  bool apQuit();
  
  bool connClose(int id);
  
  String getIp();

  String readResponse(int timeoutMs = 5000);
  
  bool readResponse(byte &data, int timeoutMs = 5000);

  void clearBuffer();

private:
  bool _sendData(int id, String data);

  bool sendCommandAndWait(String cmd, String expectedResult = "OK", int timeoutMs = 10000, bool includeCrLf = true);

  void sendCommand(String cmd, bool includeCrLf = true);
  
  bool waitForResponse(String expectedResult, int timeoutMs = 10000);
  
  String recvBuffer;
  
  void checkIPD(String str);
};

#endif
