#ifndef iocontroller_h
#define iocontroller_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "input_handler.h"

#define PIN_PUMP_HC 2
#define PIN_PUMP_HC_INDEX 0

#define PIN_PUMP_WATER 4
#define PIN_PUMP_WATER_INDEX 1

#define PIN_FLOW_SWITCH 8
#define PIN_FLOW_SWITCH_INDEX 2

#define IOC_MODULE_NAME F("IOC")

class IOController : public InputHandler::InputListener {
public:
  
  void init();
  
  void update();
  
  void setValue(int pin, int pinIndex, bool enable);
  void setValue(int pin, int pinIndex, bool enable, bool force);
  
  bool getValue(int pinIndex);
  
  void toggle(int pin, int pinIndex);
  
  byte getPinState();
  
  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);
  
private:
  byte pinState = 0;
  
  void _printState(String ioName, bool enabled);
};


#endif

