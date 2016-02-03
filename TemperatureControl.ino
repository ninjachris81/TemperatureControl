#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>
#include <RF24.h>

#include <avr/wdt.h>

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
#include "remotectrl_logic.h"
#include "errordetector_logic.h"

#define DISABLE_WIFI
#define DISABLE_BT
#define DO_LOG_DEFAULT true

LedLogic led;
Settings settings;
TimeLogic time;
TemperatureLogic temp;
IOController ioController;
SerialApi serialApi;

#ifndef DISABLE_BT
  BluetoothLogic bluetooth;
#endif
#ifndef DISABLE_WIFI
  WifiLogic wifiLogic;
#endif
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

  errorDetectorLogic.init();

  errorDetectorLogic.setIOController(&ioController);
  errorDetectorLogic.setTempLogic(&temp);
#ifndef DISABLE_WIFI
  errorDetectorLogic.setWifiLogic(&wifiLogic);
#endif

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

  wdt_enable(WDTO_8S);
}

void loop() {
  if (!doExecuteProg) return;
  
  if (lastRamUpdate==0 || millis() - lastRamUpdate >= RAM_UPDATE_INTERVAL_MS) {
    lastRamUpdate = millis();
    freeRam = getFreeRam();
    LogHandler::logMsg(MAIN_MODULE_NAME, F("RAM: "), freeRam);
  }
  
  serialApi.update();
  wdt_reset();

  led.update();
  wdt_reset();
  
  temp.update();
  wdt_reset();

  time.update();
  wdt_reset();
  
  ioController.update();
  wdt_reset();

#ifndef DISABLE_BT
  bluetooth.update();
  wdt_reset();
#endif
  
#ifndef DISABLE_WIFI
  wifiLogic.update(freeRam);
  wdt_reset();
#endif

  remoteCtrlLogic.update();
  wdt_reset();

  errorDetectorLogic.update();
  wdt_reset();
}
