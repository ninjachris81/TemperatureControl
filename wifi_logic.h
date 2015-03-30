#ifndef wifi_logic_h
#define wifi_logic_h

#include <SoftwareSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define BUFFER_SIZE 512

#define SSID  "Stalnet"      // change this to match your WiFi SSID
#define PASS  "wlandome"  // change this to match your WiFi password
#define PORT  "80"           // using port 8080 by default

#define WIFI_MODULE_NAME "Wifi"

class WifiLogic {
public:
  void init();
  
  void update();
  

private:
  SoftwareSerial *esp;
  char buffer[BUFFER_SIZE];

  bool setupWiFi();
  bool wait_for_esp_response(int timeout);
  bool wait_for_esp_response(int timeout, char* term);
  
};

#endif
