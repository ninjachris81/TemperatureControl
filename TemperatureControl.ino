#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>

#include "globals.h"

#define MAIN_MODULE_NAME F("MAIN")

#include "input_handler.h"
#include "bluetooth_logic.h"
#include "settings.h"
#include "time_logic.h"
#include "temperature_logic.h"
#include "iocontroller.h"
#include "led_logic.h"
#include "serial_api.h"
#include "wifi_logic.h"

LedLogic led;
Settings settings;
TimeLogic time;
TemperatureLogic temp;
IOController ioController;
SerialApi serialApi;
BluetoothLogic bluetooth;
WifiLogic wifiLogic;

int getFreeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void setup() {
  delay(2000);
  
  LogHandler::init(&led);
  LogHandler::logMsg(MAIN_MODULE_NAME, F("Temp Ctrl v0.1"));
  led.init();
  
  settings.init();
  settings.loadSettings();
  ioController.init(settings.settingsData.io);
  time.init();
  temp.init(settings.settingsData.temp, &ioController);

  serialApi.init();
  
  bluetooth.init();

  wifiLogic.init(&temp, &ioController);
  
  if (!LogHandler::hasFatalError) {
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Finished init successfully"));
  } else {
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Error while init"));
  }
}

void loop() {
  if (lastRamUpdate==0 || millis() - lastRamUpdate >= RAM_UPDATE_INTERVAL_MS) {
    lastRamUpdate = millis();
    freeRam = getFreeRam();
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Free RAM: "), freeRam);
  }
  
  led.update();
  
  serialApi.update();
  
  temp.update();

  time.update();
  
  bluetooth.update();
  
  ioController.update();

  wifiLogic.update(freeRam);

  //if (!LogHandler::hasFatalError) {
  //}
}
