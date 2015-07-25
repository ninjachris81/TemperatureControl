#ifndef wifi_logic_h
#define wifi_logic_h

#include "esp8266_httpclient.h"
#include "temperature_logic.h"
#include "iocontroller.h"

#define WIFI_HANDLER_MODULE_NAME "WIFI"

//#define WIFI_CHECK_INTERVAL_MIN_MS 900000   // 10 minutes
#define WIFI_CHECK_INTERVAL_MIN_MS 60000   // 1 minute

#define WIFI_STATUS_INIT 0
#define WIFI_STATUS_CONNECTED_AP 1

#define WIFI_REMOTE_IP F("184.106.153.149")
#define WIFI_REMOTE_HOST F("api.thingspeak.com")

class WifiLogic {
public:

  void init(TemperatureLogic *temperatureLogic, IOController *ioController);

  void update(int freeRam);

private:
  ESP8266 myEsp;
  ESP8266HttpClient myClient;

  TemperatureLogic *temperatureLogic;
  IOController *ioController;

  bool connectAp();
  
  void syncData(int freeRam);

  void addParam(String &query, uint8_t index, int value);
  
  void parseDate(String inputStr, uint8_t &h, uint8_t &m);

  unsigned long lastUpdate = 0;
  
  uint8_t status = WIFI_STATUS_INIT;

};

#endif
