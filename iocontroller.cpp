#include "iocontroller.h"
#include "globals.h"

void IOController::init() {
  LogHandler::logMsg(IOC_MODULE_NAME, F("IO Controller init"));
  pinMode(PIN_PUMP, OUTPUT);
  setValue(PIN_PUMP, PIN_PUMP_INDEX, false);
  InputHandler::registerListener(this);
}

void IOController::setValue(int pin, int pinIndex, bool enable) {
  bool value = bitRead(pinState, pinIndex);
  if (value==enable) return;    // already set

  String msg = F("Setting PIN ");
  msg.concat(pin);
  msg.concat(F(" to "));
  msg.concat((enable ? F("true") : F("false")));
  LogHandler::logMsg(IOC_MODULE_NAME, msg);
  
  digitalWrite(pin, !enable);    // map HIGH to off, LOW to on
  if (enable) {
    bitSet(pinState, pinIndex);
  } else {
    bitClear(pinState, pinIndex);
  }
}

String IOController::getName() {
  return IOC_MODULE_NAME;
}

bool IOController::onInput(String cmd) {
  if (cmd.equals(F("ON"))) {
    setValue(PIN_PUMP, PIN_PUMP_INDEX, true);
    return true;
  } else if (cmd.equals(F("OFF"))) {
    setValue(PIN_PUMP, PIN_PUMP_INDEX, false);
    return true;
  } else if (cmd.equals(F("TOGGLE"))) {
    toggle(PIN_PUMP, PIN_PUMP_INDEX);
    return true;
  } else if (cmd.equals(F("GET"))) {
    bool enable = bitRead(PIN_PUMP, PIN_PUMP_INDEX);
    String msg = F("PUMP is ");
    msg.concat((enable ? F("enabled") : F("disabled")));
    LogHandler::logMsg(IOC_MODULE_NAME, msg);
    return true;
  }
  
  return false;
}

void IOController::toggle(int pin, int pinIndex) {
  bool value = bitRead(pinState, pinIndex);
  setValue(pin, pinIndex, !value);
}

byte IOController::getPinState() {
  return pinState;
}


