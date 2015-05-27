#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>

#include "globals.h"

#define MAIN_MODULE_NAME F("MAIN")

#include "settings.h"
#include "time_logic.h"
#include "temperature_logic.h"
#include "iocontroller.h"
#include "led_logic.h"
#include "serial_api.h"
#include "bluetooth_logic.h"

LedLogic led;
Settings settings;
TimeLogic time;
TemperatureLogic temp;
IOController ioController;
SerialApi serialApi;
BluetoothLogic bluetooth;

void setup() {
  LogHandler::init(&led);
  LogHandler::logMsg(MAIN_MODULE_NAME, F("Temperature Control v0.1"));
  led.init();
  
  settings.init();
  settings.loadSettings();
  ioController.init(settings.settingsData.io);
  time.init();
  temp.init(settings.settingsData.temp, &ioController);

  serialApi.init();
  
  bluetooth.init();
  
  if (!LogHandler::hasFatalError) {
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Finished init successfully"));
  } else {
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Error while init"));
  }
}

void loop() {
  led.update();
  
  serialApi.update();
  
  temp.update();

  time.update();
  
  bluetooth.update();
  
  ioController.update();

  //if (!LogHandler::hasFatalError) {
  //}
}
