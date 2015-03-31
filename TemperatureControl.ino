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
#include "input_handler.h"

LedLogic led;
Settings settings;
TimeLogic time;
TemperatureLogic temp;
IOController ioController;
WifiLogic wifiLogic;
InputHandler inputHandler;

void setup() {
  LogHandler::init();
  LogHandler::logMsg("MAIN", F("Temperature Control v0.1"));
  led.init();
  ErrorHandler::init(&led);
  
  settings.loadSettings();
  ioController.init();
  time.init();
  temp.init(settings.settingsData.temp, &ioController);
  wifiLogic.init();
  inputHandler.init();
  
  if (!ErrorHandler::hasFatalError) {
    LogHandler::logMsg("MAIN", F("Finished init successfully"));
  } else {
    LogHandler::logMsg("MAIN", F("Error while init"));
  }
}

void loop() {
  led.update();
  
  inputHandler.update();
  
  if (!ErrorHandler::hasFatalError) {
    time.update();
  
    temp.update();
    
    wifiLogic.update();
  }
}
