#ifndef bluetooth_logic_h
#define bluetooth_logic_h

#include <SoftwareSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "temperature_logic.h"
#include "iocontroller.h"
#include "settings.h"
#include "log_handler.h"

#define BT_RX_PIN 10
#define BT_TX_PIN 11
#define BT_BAUD_RATE 9600

#define INPUT_HANDLER_MODULE_NAME F("Bluetooth")

#define UPDATE_INTERVAL_MS 2000

#define BT_NAME F("TEMP_CTRL")

class BluetoothLogic : public LogHandler::LogListener {
public:
  void init(TemperatureLogic *tempLogic, IOController* ioController, Settings *settings);
  
  void update();

  /*virtual*/ void onMessage(String msg, LogHandler::LOG_TYPE type=LogHandler::LOG);
  
private:
  unsigned long lastUpdate = 0;
  
  TemperatureLogic *tempLogic;
  IOController* ioController;
  Settings *settings;
  
  char currentMode = '0';

  void sendData();

};


#endif
