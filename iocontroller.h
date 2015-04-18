#ifndef iocontroller_h
#define iocontroller_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define PIN_PUMP 7
#define PIN_PUMP_INDEX 0
#define IOC_MODULE_NAME F("IOC")

class IOController {
public:
  
  void init();
  
  void setValue(int pin, int pinIndex, bool enable);
  
  void toggle(int pin, int pinIndex);
  
  byte getPinState();
  
private:
  byte pinState = 0;
  
};


#endif

