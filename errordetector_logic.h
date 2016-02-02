#ifndef ERRORDETECTOR_LOGIC_H
#define ERRORDETECTOR_LOGIC_H

#include "globals.h"
#include "iocontroller.h"
#include "wifi_logic.h"
#include "temperature_logic.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define ERRDECTECT_HANDLER_MODULE_NAME MODNAME("ERRD")

#define ERROR_CHECK_INTERVAL 10000

#define ERROR_COUNT 3
#define ERROR_INDEX_HC 0
#define ERROR_INDEX_TEMP_WATER 1
#define ERROR_INDEX_TEMP_HC 2

#define ERROR_TIMEOUT_HC_MS 3600000   // 1h
#define ERROR_TIMEOUT_TEMP_MS 60000   // 1min

class ErrorDetectorLogic {
  public:

  void init();

  void update();

  void setIOController(IOController *ioController);
  void setWifiLogic(WifiLogic *wifiLogic);
  void setTempLogic(TemperatureLogic *tempLogic);

private:
  IOController *ioController = NULL;
  WifiLogic *wifiLogic = NULL;
  TemperatureLogic *tempLogic = NULL;
  
  unsigned long lastUpdate = 0;
  
  unsigned long durations[ERROR_COUNT];
  unsigned long max_durations[ERROR_COUNT];
  bool errors[ERROR_COUNT];

  void checkErrors();

  void checkError(bool hasError, uint8_t index);

  void reportError(uint8_t type);

  byte currentValue;
  
};

#endif
