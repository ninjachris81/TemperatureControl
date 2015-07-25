#include "iocontroller.h"
#include "globals.h"
#include "log_handler.h"
#include "output_handler.h"

void IOController::init(IOSettingsStruct &settings) {
  //LogHandler::logMsg(IOC_MODULE_NAME, F("Init"));
  
  this->settingsData = settings;

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
  switch(settingsData.ioModes[pinIndex]) {
    case PUMP_MODE_ON:
      enable = true;
      break;
    case PUMP_MODE_OFF:
      enable = false;
      break;
    case PUMP_MODE_AUTO:
      // default behavior
      break;
  }

  bool value = bitRead(this->pinState, pinIndex);
  if (!force && value==enable) return;    // already set
  
  LogHandler::logMsg(IOC_MODULE_NAME, F("MODE: "), settingsData.ioModes[pinIndex]);

  String msg = F("Set PIN ");
  msg.concat(pin);
  msg.concat(F(" to "));
  msg.concat((enable ? ENABLED_STRING : DISABLED_STRING));
  LogHandler::logMsg(IOC_MODULE_NAME, msg);
  
  digitalWrite(pin, !enable);    // map HIGH to off, LOW to on
  if (enable) {
    bitSet(this->pinState, pinIndex);
  } else {
    bitClear(this->pinState, pinIndex);
  }
  
  _sendIOState();
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
  if (cmd.equals(F("GET"))) {
    _sendIOState();
    return true;
  } else {
    int v2, v3;
    String v1;
    if (InputHandler::parseParameters3(cmd, v1, v2, v3)) {
      uint8_t ioDevice;
      uint8_t ioIndex;
      
      if (v2==0) {
        ioDevice = PIN_PUMP_HC;
        ioIndex = PIN_PUMP_HC_INDEX;
      } else  if (v2==1) {
        ioDevice = PIN_PUMP_WATER;
        ioIndex = PIN_PUMP_WATER_INDEX;
      } else  if (v2==2) {
        ioDevice = PIN_FLOW_SWITCH;
        ioIndex = PIN_FLOW_SWITCH_INDEX;
      } else {
        LogHandler::warning(IOC_MODULE_NAME, F("Inv index"));
        return false;
      }
      
      if (v1==F("MODE")) {
        if (v3>=PUMP_MODE_OFF&&v3<=PUMP_MODE_AUTO) {
          LogHandler::logMsg(IOC_MODULE_NAME, F("Set mode: "), v3);
          settingsData.ioModes[ioIndex] = v3;
          setValue(ioDevice, ioIndex, v2==1);
          return true;
        } else {
          LogHandler::warning(IOC_MODULE_NAME, F("Inv mode"));
          return false;
        }
      } else {
        return false;
      }
    } else {
      LogHandler::warning(IOC_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
    }
    
    return true;
  }
  
  return false;
}

void IOController::_sendIOState() {
  String tmpStr = F("IOST ");
  
  tmpStr.concat(getValue(PIN_PUMP_HC_INDEX) ? 1 : 0);
  tmpStr.concat(F(" "));
  tmpStr.concat(getValue(PIN_PUMP_WATER_INDEX) ? 1 : 0);
  tmpStr.concat(F(" "));
  tmpStr.concat(getValue(PIN_FLOW_SWITCH_INDEX) ? 1 : 0);
  
  OutputHandler::sendCmd(IOC_MODULE_NAME, tmpStr);

  _printState(F("TANK PUMP"), getValue(PIN_PUMP_HC_INDEX));
  _printState(F("WATER PUMP"), getValue(PIN_PUMP_WATER_INDEX));
  _printState(F("FLOW SWITCH"), getValue(PIN_FLOW_SWITCH_INDEX));
}

void IOController::update() {
  if (settingsData.ioModes[PIN_FLOW_SWITCH_INDEX]!=PUMP_MODE_AUTO) return;
  
  int val = !digitalRead(PIN_FLOW_SWITCH);
  bool orgVal = getValue(PIN_FLOW_SWITCH_INDEX);
  
  if (val==orgVal) return;
  
  _sendIOState();
  
  if (val==HIGH) {
//    LogHandler::logMsg(IOC_MODULE_NAME, F("FLOW SWITCH is ON"));
    bitSet(this->pinState, PIN_FLOW_SWITCH_INDEX);
  } else {
//    LogHandler::logMsg(IOC_MODULE_NAME, F("FLOW SWITCH is OFF"));
    bitClear(this->pinState, PIN_FLOW_SWITCH_INDEX);
  }
}
