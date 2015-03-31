#include "iocontroller.h"
#include "globals.h"

void IOController::init() {
  LogHandler::logMsg(IOC_MODULE_NAME, F("IO Controller init"));
  pinMode(PIN_PUMP, OUTPUT);
}

void IOController::setValue(int pin, int pinIndex, bool enable) {
  bool value = bitRead(pinState, pinIndex);
  if (value==enable) return;    // already set

  String msg = "Setting PIN " + pin;
  msg.concat(" to ");
  msg.concat((enable ? "true" : "false"));
  LogHandler::logMsg(IOC_MODULE_NAME, msg);
  
  digitalWrite(pin, enable);
  if (enable) {
    bitSet(pinState, pinIndex);
  } else {
    bitClear(pinState, pinIndex);
  }
}

void IOController::toggle(int pin, int pinIndex) {
  bool value = bitRead(pinState, pinIndex);
  setValue(pin, pinIndex, !value);
}

