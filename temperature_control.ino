#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>

#include "globals.h"

#include "settings.h"
#include "time_logic.h"
#include "temperature_logic.h"
#include "iocontroller.h"
#include "wifi_logic.h"
#include "led_logic.h"

LedLogic led;
Settings settings;
TimeLogic time;
TemperatureLogic temp;
IOController ioController;
WifiLogic wifiLogic;

void setup() {
  LogHandler::init();
  LogHandler::logMsg("MAIN", "Temperature Control v0.1");
  led.init();
  ErrorHandler::init(&led);

  settings.loadSettings();
  ioController.init();
  time.init();
  temp.init(settings.settingsData.temp, &ioController);
  wifiLogic.init();
  
  if (!ErrorHandler::hasFatalError) {
    LogHandler::logMsg("MAIN", "Finished init successfully");
  } else {
    LogHandler::logMsg("MAIN", "Error while init");
  }
}

void loop() {
  led.update();
  
  if (!ErrorHandler::hasFatalError) {
    time.update();
  
    //temp.update();
    
    //wifiLogic.update();
  }
}
