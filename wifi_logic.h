#ifndef wifi_logic_h
#define wifi_logic_h

#include "globals.h"
#include "temperature_logic.h"
#include "input_handler.h"
#include "iocontroller.h"
#include "property.h"
#include "stop_timer.h"

#define WIFI_HANDLER_MODULE_NAME MODNAME("WIFI")

#define WIFI_UPDATE_INTERVAL_MIN_MS 10000

#define WIFI_OVERRIDE_TIMEOUT_MS 60000

#define ESP_SERIAL Serial1

// @see TemperatureControlESP8266 -> http_logic.h
#define FIELD_INDEX_WATER 0
#define FIELD_INDEX_HC 1
#define FIELD_INDEX_TANK 2
#define FIELD_INDEX_PUMP_WATER_SEC 3
#define FIELD_INDEX_PUMP_HC_SEC 4
#define FIELD_INDEX_FLOW_SWITCH 5
#define FIELD_INDEX_FREE_RAM 6
#define FIELD_INDEX_RESTARTED 7
#define FIELD_INDEX_PUMP_WATER_ON 8
#define FIELD_INDEX_PUMP_HC_ON 9

class WifiLogic: public InputHandler::InputListener, public Property::ValueChangeListener {
public:

  struct WifiSettingsStruct {
    byte apIndex;   // obsolete
  } *settingsData;
  
  void init(WifiSettingsStruct *settings, TemperatureLogic *temperatureLogic, IOController *ioController);

  void update(int freeRam);

  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);

  void reportError(String errorMsg);

private:
  unsigned long lastOverrideReceived = 0;
  int lastFreeRam = 0;

  TemperatureLogic *temperatureLogic;
  IOController *ioController;

  /*virtual*/ void onPropertyValueChange(uint8_t id, int value);
  void setStopTimer(StopTimer &timer, int value);

  StopTimer hcST;
  StopTimer wST;
  StopTimer fsST;

  void setManualOverride(bool enabled);

  void updateFieldValue(uint8_t index, int value);

  void setUpdateActive(bool isActive);

  void setDataActive(bool isActive);

  bool firstTime = true;

  bool isDataActive = false;

  unsigned long lastUpdate = 0;
  
};

#endif
