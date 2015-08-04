#ifndef WATCHDOG_LOGIC_H
#define WATCHDOG_LOGIC_H

#include "globals.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#define SEND_INTERVAL_MIN_MS 5000

class WatchdogLogic {
  public:

  void init();

  void update();

private:
  unsigned long lastUpdate = 0;

  byte currentValue;
  
};

#endif
