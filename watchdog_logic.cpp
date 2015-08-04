#include "watchdog_logic.h"

void WatchdogLogic::init() {
  currentValue = LOW;
  
  pinMode(PIN_WATCHDOG_COMM, OUTPUT);
}

void WatchdogLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= SEND_INTERVAL_MIN_MS) {
    
    if (currentValue==LOW) {
      currentValue = HIGH;
    } else {
      currentValue = LOW;
    }

    digitalWrite(PIN_WATCHDOG_COMM, currentValue);   
  }
}

