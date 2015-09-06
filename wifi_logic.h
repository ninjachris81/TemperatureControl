#ifndef wifi_logic_h
#define wifi_logic_h

#include "globals.h"
#include "esp8266_httpclient.h"
#include "temperature_logic.h"
#include "input_handler.h"
#include "iocontroller.h"
#include "property.h"
#include "stop_timer.h"

#define WIFI_HANDLER_MODULE_NAME MODNAME("WIFI")

#define WIFI_AP_STALNET_REPEAT 0
#define WIFI_AP_STALNET 1

//#define WIFI_CHECK_INTERVAL_MIN_MS 300000   // 5 minutes
#define WIFI_CHECK_INTERVAL_MIN_MS 600000   // 10 minutes
//#define WIFI_CHECK_INTERVAL_MIN_MS 60000   // 1 minute

#define WIFI_STATUS_INIT 0
#define WIFI_STATUS_CONNECTED_AP 1

#define WIFI_REMOTE_IP F("184.106.153.149")
#define WIFI_REMOTE_HOST F("api.thingspeak.com")

class WifiLogic: public InputHandler::InputListener, public Property::ValueChangeListener {
public:

  struct WifiSettingsStruct {
    byte apIndex;
  } *settingsData;
  
  void init(WifiSettingsStruct *settings, TemperatureLogic *temperatureLogic, IOController *ioController);

  void update(int freeRam);

  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);

private:
  ESP8266 myEsp;
  ESP8266HttpClient myClient;

  TemperatureLogic *temperatureLogic;
  IOController *ioController;

  /*virtual*/ void onPropertyValueChange(uint8_t id, int value);
  void setStopTimer(StopTimer &timer, int value);

  StopTimer hcST;
  StopTimer wST;
  StopTimer fsST;

  bool firstTime = true;

  bool connectAp();
  
  void syncData(int freeRam);

  void addParam(String &query, uint8_t index, int value);
  
  void parseDate(String inputStr, uint8_t &h, uint8_t &m);

  unsigned long lastUpdate = 0;
  
  uint8_t status = WIFI_STATUS_INIT;

};

#endif
