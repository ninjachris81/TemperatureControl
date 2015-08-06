#include "watchdog_logic.h"
#include "log_handler.h"

void WatchdogLogic::init() {
  currentValue = LOW;
  
  pinMode(PIN_WATCHDOG_COMM, OUTPUT);
}

void WatchdogLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= SEND_INTERVAL_MIN_MS) {

    LogHandler::logMsg(WDOG_HANDLER_MODULE_NAME, F("ping"));
    
    if (currentValue==LOW) {
      currentValue = HIGH;
    } else {
      currentValue = LOW;
    }

    digitalWrite(PIN_WATCHDOG_COMM, currentValue);   
    lastUpdate = millis();
  }
}

