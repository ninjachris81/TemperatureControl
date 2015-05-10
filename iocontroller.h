#ifndef iocontroller_h
#define iocontroller_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "input_handler.h"

#define PIN_PUMP 2
#define PIN_PUMP_INDEX 0
#define IOC_MODULE_NAME F("IOC")

class IOController : public InputHandler::InputListener {
public:
  
  void init();
  
  void setValue(int pin, int pinIndex, bool enable);
  
  void toggle(int pin, int pinIndex);
  
  byte getPinState();
  
  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);
  
private:
  byte pinState = 0;
  
};


#endif

