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

#define ESP_DEFAULT_REQUEST_TIMEOUT_MS 5000
#define ESP_SEND_DATA_TIMEOUT_FACTOR 3

#define ESP_TIMEOUT_MS 100
#define ESP_BAUD_RATE 115200
//#define ESP_BAUD_RATE 921600
//#define ESP_BAUD_RATE 9600

#define CONN_MODE_SINGLE 0
#define CONN_MODE_MULTI 1

#define NETWORK_MODE_STA 1
#define NETWORK_MODE_AP 2
#define NETWORK_MODE_BOTH 3

#define SERVER_MODE_CLOSE 0
#define SERVER_MODE_OPEN 1

#define TCP_MODE F("TCP")
#define UDP_MODE F("UDP")

#define SEND_BUFFER_SIZE 64

#define ESP_TASK_TYPE_NONE 0
#define ESP_TASK_TYPE_TEST 1
#define ESP_TASK_TYPE_SOFT_RESET 2
#define ESP_TASK_TYPE_ECHO_OFF 3
#define ESP_TASK_TYPE_NETWORK_MODE 4
#define ESP_TASK_TYPE_AP_JOIN 5
#define ESP_TASK_TYPE_CONN_MODE 6
#define ESP_TASK_TYPE_CONNECT_TO_SERVER 7
#define ESP_TASK_TYPE_SEND_DATA 8
#define ESP_TASK_TYPE_CLOSE_CONN 9
#define ESP_TASK_TYPE_AP_QUIT 10

#define ESP_STATE_SEND_WAIT_FOR_READY 1
#define ESP_STATE_SEND_WAIT_FOR_SENT_OK 2
#define ESP_STATE_SEND_NEXT_PACKET 3

class ESP8266 {
public:

  class TaskCallbackHandler {
  public:
    virtual void softResetComplete(bool success) = 0;
    virtual void echoOffComplete(bool success) = 0;
    virtual void testComplete(bool success) = 0;
    virtual void networkModeComplete(bool success) = 0;
    virtual void apJoinComplete(bool success) = 0;
    virtual void connModeComplete(bool success) = 0;
    virtual void connectToServerComplete(bool success) = 0;
    virtual void sendDataComplete(bool success) = 0;
    virtual void closeConnComplete(bool success) = 0;
    virtual void apQuitComplete(bool success) = 0;
  };

  struct ESPTask {
    unsigned long startedAt;
    uint16_t currentTimeout;
    bool isTimeout;
    
    uint8_t type;
    uint8_t id;
    uint8_t state;
    String dataToSend;
    uint16_t sendOffset;
  };

  void init();

  void setTaskCallbackHandler(TaskCallbackHandler* tcbh);
  
  void update();
  
  void setBaudRate();
  
  bool setTimeout(int timeoutMs);
  
  void test();
  
  void softReset();
  
  void echoOff();
  
  void getStatus();

  void setNetworkMode(uint8_t mode);
  
  void setConnMode(uint8_t mode);
  
  bool serverMode(uint8_t mode, int port = -1);
  
  void connectToServer(int id, String type, String ip, uint16_t port);
  
  bool sendData(int id, String data);

  bool apInRange(String name);
  
  void apJoin(String name, String password);
  
  void apQuit();
  
  void closeConn(int id);
  
  String getIp();

  String readResponse(int timeoutMs = 5000);
  
  bool readResponse(byte &data, int timeoutMs = 5000);

  void clearBuffer();

  void printTask();

private:
  TaskCallbackHandler* tcbh;
  ESPTask currentTask;

  void clearTask();

  void _softReset();

  void _echoOff();

  void _test();

  void _setNetworkMode();

  void _apJoin();

  void _apQuit();

  void _setConnMode();

  void _connectToServer();

  void _sendData();

  void _sendDataInit(int id, int dataSize);

  void _sendNextDataPacket();

  void _closeConn();

  bool sendCommandAndWait(String cmd, String expectedResult = "OK", int timeoutMs = 10000, bool includeCrLf = true);

  void sendCommand(String cmd, bool includeCrLf = true);
  
  bool waitForResponse(String expectedResult = "OK", int timeoutMs = 100);

  String recvBuffer;
  
  void checkIPD(String str);
};

#endif
