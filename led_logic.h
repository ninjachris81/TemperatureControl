#ifndef led_logic_h
#define led_logic_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define LED_PIN 13
#define LED_INTERVAL_MS 2000

class LedLogic {
public:  
  void init();
  
  void update();
  
  void setInterval(int interval);
  
private:
  bool enabled;
  
  unsigned long lastUpdate = 0;
  
  int interval;
};

#endif
