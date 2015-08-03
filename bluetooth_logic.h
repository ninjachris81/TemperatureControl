#ifndef bluetooth_logic_h
#define bluetooth_logic_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "globals.h"
#include "log_handler.h"
#include "input_handler.h"
#include "output_handler.h"

#define BT_BAUD_RATE 9600

#define BT_MODULE_NAME MODNAME("BT")

#define UPDATE_INTERVAL_MS 2000

#define BT_NAME F("TEMP_CTRL")

#define BT_CMD_SEP F(";")

#define BT_AUTH_HASH 14624

class BluetoothLogic : public LogHandler::LogListener, public InputHandler::InputListener, public OutputHandler::OutputSink  {
public:
  void init();
  
  void update();

  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);
  
  /*virtual*/ void onLogMessage(String msg, LogHandler::LOG_TYPE type=LogHandler::LOG);
  
  /*virtual*/ void sendCmd(String sender, String cmd);

private:
  bool sendLogUpdates;
  bool sendDataUpdates;
  
  bool isAuthenticated;
  
  bool doRead;
  
};


#endif
