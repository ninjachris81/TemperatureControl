#include "iocontroller.h"
#include "globals.h"

void IOController::init() {
  LogHandler::logMsg(IOC_MODULE_NAME, F("IO Controller init"));

  pinMode(PIN_PUMP_HC, OUTPUT);
  setValue(PIN_PUMP_HC, PIN_PUMP_HC_INDEX, false, true);
  pinMode(PIN_PUMP_WATER, OUTPUT);
  setValue(PIN_PUMP_WATER, PIN_PUMP_WATER_INDEX, false, true);
  pinMode(PIN_FLOW_SWITCH, INPUT);
  bitClear(this->pinState, PIN_FLOW_SWITCH_INDEX);

  InputHandler::registerListener(this);
}

bool IOController::getValue(int pinIndex) {
  return bitRead(this->pinState, pinIndex);
}

void IOController::setValue(int pin, int pinIndex, bool enable) {
  setValue(pin, pinIndex, enable, false);
}

void IOController::setValue(int pin, int pinIndex, bool enable, bool force) {
  bool value = bitRead(this->pinState, pinIndex);
  if (!force && value==enable) return;    // already set

  String msg = F("Setting PIN ");
  msg.concat(pin);
  msg.concat(F(" to "));
  msg.concat((enable ? F("true") : F("false")));
  LogHandler::logMsg(IOC_MODULE_NAME, msg);
  
  digitalWrite(pin, !enable);    // map HIGH to off, LOW to on
  if (enable) {
    bitSet(this->pinState, pinIndex);
  } else {
    bitClear(this->pinState, pinIndex);
  }
}

String IOController::getName() {
  return IOC_MODULE_NAME;
}

void IOController::_printState(String ioName, bool enabled) {
  String msg = ioName;
  msg.concat(F(" is "));
  msg.concat((enabled ? ENABLED_STRING : DISABLED_STRING));
  LogHandler::logMsg(IOC_MODULE_NAME, msg);
}

bool IOController::onInput(String cmd) {
  int v1, v2;
  
  if (cmd.equals(F("GET"))) {
      bool enabled1 = bitRead(PIN_PUMP_HC, PIN_PUMP_HC_INDEX);
      bool enabled2 = bitRead(PIN_PUMP_WATER, PIN_PUMP_WATER_INDEX);
      
      _printState(F("TANK PUMP"), enabled1);
      _printState(F("WATER PUMP"), enabled2);
      _printState(F("FLOW SWITCH"), bitRead(this->pinState, PIN_FLOW_SWITCH_INDEX));
      return true;
  } else {
    if (InputHandler::parseParameters2(cmd, v1, v2)) {
      uint8_t pump;
      uint8_t pumpIndex;
      
      if (v1==1) {
        pump = PIN_PUMP_HC;
        pumpIndex = PIN_PUMP_HC_INDEX;
      } else  if (v1==2) {
        pump = PIN_PUMP_WATER;
        pumpIndex = PIN_PUMP_WATER_INDEX;
      } else {
        LogHandler::warning(IOC_MODULE_NAME, F("Invalid pump index !"));
        return false;
      }

      setValue(pump, pumpIndex, v2==1);
      return true;

    } else {
      LogHandler::warning(IOC_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
    }
    
    return true;
  }
  
  return false;
}

void IOController::update() {
  int val = !digitalRead(PIN_FLOW_SWITCH);
  
  if (val==HIGH) {
//    LogHandler::logMsg(IOC_MODULE_NAME, F("FLOW SWITCH is ON"));
    bitSet(this->pinState, PIN_FLOW_SWITCH_INDEX);
  } else {
//    LogHandler::logMsg(IOC_MODULE_NAME, F("FLOW SWITCH is OFF"));
    bitClear(this->pinState, PIN_FLOW_SWITCH_INDEX);
  }
}


void IOController::toggle(int pin, int pinIndex) {
  bool value = bitRead(pinState, pinIndex);
  setValue(pin, pinIndex, !value);
}

byte IOController::getPinState() {
  return pinState;
}


