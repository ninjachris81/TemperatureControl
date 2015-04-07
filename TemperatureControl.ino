#include <SPI.h>
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
#include "display_logic.h"

LedLogic led;
Settings settings;
TimeLogic time;
TemperatureLogic temp;
IOController ioController;
WifiLogic wifi;
InputHandler inputHandler;
DisplayLogic displayLogic;

void setup() {
  LogHandler::init();
  LogHandler::logMsg("MAIN", F("Temperature Control v0.1"));
  led.init();
  ErrorHandler::init(&led);
  
  settings.loadSettings();
  ioController.init();
  time.init();
  temp.init(settings.settingsData.temp, &ioController);
  wifi.init();
  inputHandler.init(&settings, &temp, &time);
  
  displayLogic.init();
  
  if (!ErrorHandler::hasFatalError) {
    LogHandler::logMsg("MAIN", F("Finished init successfully"));
  } else {
    LogHandler::logMsg("MAIN", F("Error while init"));
  }
}

void loop() {
  led.update();
  
  inputHandler.update();
  
  displayLogic.update();
  
  temp.update();

  time.update();

  if (!ErrorHandler::hasFatalError) {
    temp.update();
    
    wifi.update();
  }
}
