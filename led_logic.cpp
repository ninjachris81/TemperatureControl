#include "led_logic.h"

void LedLogic::init() {
  this->interval = LED_INTERVAL_MS;
  pinMode(LED_PIN, OUTPUT);
  enabled = false;
}

void LedLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= this->interval) {      // last update check interval
    enabled = !enabled;
    digitalWrite(LED_PIN, (enabled ? HIGH : LOW));
    lastUpdate = millis();
  }
}

void LedLogic::setInterval(int interval) {
  this->interval = interval;
}

