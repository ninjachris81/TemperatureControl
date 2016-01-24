#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>
#include <RF24.h>

#include "globals.h"

#define MAIN_MODULE_NAME MODNAME("MAIN")

#include "error_handler.h"
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
#include "remotectrl_logic.h"
#include "errordetector_logic.h"

//#define DISABLE_WIFI
//#define DISABLE_BT
#define DO_LOG_DEFAULT false

LedLogic led;
Settings settings;
TimeLogic time;
TemperatureLogic temp;
IOController ioController;
SerialApi serialApi;
BluetoothLogic bluetooth;
WifiLogic wifiLogic;
WatchdogLogic watchdogLogic;
RemoteCtrlLogic remoteCtrlLogic;
ErrorDetectorLogic errorDetectorLogic;

bool doExecuteProg;

int getFreeRam()
{
  extern int __heap_start, *__brkval;
  int v;

  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void softReset() {
  asm volatile ("  jmp 0");
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

#ifndef DISABLE_BT
  bluetooth.init();
#endif

#ifndef DISABLE_WIFI
  wifiLogic.init(&settings.settingsData.wifi, &temp, &ioController);
#endif

  remoteCtrlLogic.init(&ioController, &temp);

  watchdogLogic.init();

  errorDetectorLogic.init(&ioController, &wifiLogic, &temp);
  
  if (!ErrorHandler::hasFatalErrors()) {
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
  LogHandler::doLog = DO_LOG_DEFAULT;
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
  
  ioController.update();

#ifndef DISABLE_BT
  bluetooth.update();
#endif
  
#ifndef DISABLE_WIFI
  wifiLogic.update(freeRam);
#endif

  remoteCtrlLogic.update();

  errorDetectorLogic.update();

  if (!ErrorHandler::hasFatalErrors()) {
    watchdogLogic.update();
  }
}
