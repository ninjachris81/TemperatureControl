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

#define WIFI_REMOTE_IP F("184.106.153.149")
#define WIFI_REMOTE_HOST F("api.thingspeak.com")

class WifiLogic: public InputHandler::InputListener, public Property::ValueChangeListener, public ESP8266HttpClient::HttpClientTaskCallbackHandler {
public:

  struct WifiSettingsStruct {
    byte apIndex;
  } *settingsData;
  
  void init(WifiSettingsStruct *settings, TemperatureLogic *temperatureLogic, IOController *ioController);

  void update(int freeRam);

  /*virtual*/ void initComplete(bool success);
  /*virtual*/ void connectToServerComplete(bool success);
  /*virtual*/ void executeGETComplete(bool success, String response);
  /*virtual*/ void disconnectComplete(bool success);
  /*virtual*/ void deinitComplete(bool success);

  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);

private:
  ESP8266HttpClient myClient;
  int freeRam;

  bool readyToSend;
  
  TemperatureLogic *temperatureLogic;
  IOController *ioController;

  void initConnection();

  String buildQuery(int freeRam);

  /*virtual*/ void onPropertyValueChange(uint8_t id, int value);
  void setStopTimer(StopTimer &timer, int value);

  StopTimer hcST;
  StopTimer wST;
  StopTimer fsST;

  bool firstTime = true;
  
  void addParam(String &query, uint8_t index, int value);
  
  void parseDate(String inputStr, uint8_t &h, uint8_t &m);

  unsigned long lastUpdate = 0;

};

#endif
