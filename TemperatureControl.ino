#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>

#include "globals.h"

#define MAIN_MODULE_NAME MODNAME("MAIN")

#include "input_handler.h"
#include "bluetooth_logic.h"
#include "settings.h"
#include "time_logic.h"
#include "temperature_logic.h"
#include "iocontroller.h"
#include "led_logic.h"
#include "serial_api.h"
#include "wifi_logic.h"
#include "watchdog_logic.h"

LedLogic led;
Settings settings;
TimeLogic time;
TemperatureLogic temp;
IOController ioController;
SerialApi serialApi;
BluetoothLogic bluetooth;
WifiLogic wifiLogic;
WatchdogLogic watchdogLogic;

bool doExecuteProg;

int getFreeRam()
{
  extern int __heap_start, *__brkval;
  int v;

  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void setup() {
  delay(1000);

  LogHandler::init();
  LogHandler::logMsg(MAIN_MODULE_NAME, F("Temp Ctrl v0.1"));
  led.init();
  
  settings.init();
  settings.loadSettings();
  ioController.init(&settings.settingsData.io);
  time.init();
  temp.init(&settings.settingsData.temp, &ioController);

  serialApi.init();
  
  bluetooth.init();

  wifiLogic.init(&settings.settingsData.wifi, &temp, &ioController);

  watchdogLogic.init();
  
  if (!LogHandler::hasFatalError) {
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Finished init"));
  } else {
    led.setInterval(LED_ERROR_INTERVAL_MS);
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Error @ init"));
  }

  LogHandler::logMsg(MAIN_MODULE_NAME, F("Press any key to interrupt startup"));
  delay(3000);
  doExecuteProg = (Serial.read()==-1);

  if (!doExecuteProg) {
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Startup interrupt"));
  } else {
    LogHandler::logMsg(MAIN_MODULE_NAME, F("Starting"));
  }
  LogHandler::doLog = false;
}

void loop() {
  if (!doExecuteProg) return;
  
  if (lastRamUpdate==0 || millis() - lastRamUpdate >= RAM_UPDATE_INTERVAL_MS) {
    lastRamUpdate = millis();
    freeRam = getFreeRam();
    LogHandler::logMsg(MAIN_MODULE_NAME, F("RAM: "), freeRam);
  }
  
  serialApi.update();

  led.update();
  
  temp.update();

  time.update();
  
  bluetooth.update();
  
  ioController.update();

  wifiLogic.update(freeRam);

  watchdogLogic.update();

  //if (!LogHandler::hasFatalError) {
  //}
}
