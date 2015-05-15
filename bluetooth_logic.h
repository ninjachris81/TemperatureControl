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

#define BT_MODULE_NAME F("BT")

#define UPDATE_INTERVAL_MS 2000

#define BT_NAME F("TEMP_CTRL")

#define BT_CMD_SEP F(";")

class BluetoothLogic : public LogHandler::LogListener, public InputHandler::InputListener  {
public:
  void init(TemperatureLogic *tempLogic, IOController* ioController, Settings *settings);
  
  void update();

  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);
  
  /*virtual*/ void onMessage(String msg, LogHandler::LOG_TYPE type=LogHandler::LOG);
  
private:
  unsigned long lastUpdate = 0;
  
  TemperatureLogic *tempLogic;
  IOController* ioController;
  Settings *settings;
  
  bool sendLogUpdates;
  bool sendDataUpdates;
  
  void sendData();

};


#endif
