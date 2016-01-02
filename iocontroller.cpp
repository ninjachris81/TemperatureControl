#include "iocontroller.h"
#include "globals.h"
#include "log_handler.h"
#include "output_handler.h"

void IOController::init(IOSettingsStruct *settings) {
  //LogHandler::logMsg(IOC_MODULE_NAME, F("Init"));
  
  this->settingsData = settings;

  this->hcState.init(PIN_PUMP_HC_INDEX);
  this->wState.init(PIN_PUMP_WATER_INDEX);
  this->fsState.init(PIN_FLOW_SWITCH_INDEX);

  pinMode(PIN_PUMP_HC, OUTPUT);
  setValue(PIN_PUMP_HC, PIN_PUMP_HC_INDEX, false, true);
  pinMode(PIN_PUMP_WATER, OUTPUT);
  setValue(PIN_PUMP_WATER, PIN_PUMP_WATER_INDEX, false, true);
  pinMode(PIN_FLOW_SWITCH, INPUT);
  setPropertyValue(PIN_FLOW_SWITCH_INDEX, IO_STATE_OFF);

  InputHandler::registerListener(this);

  this->hcState.registerValueChangeListener(this);
  this->wState.registerValueChangeListener(this);
  this->fsState.registerValueChangeListener(this);
}

void IOController::onPropertyValueChange(uint8_t id, int value) {
  _sendIOState();
}

void IOController::setValue(int pin, int pinIndex, bool enable) {
  setValue(pin, pinIndex, enable, false);
}

void IOController::setValue(int pin, int pinIndex, bool enable, bool force) {
  switch(settingsData->ioModes[pinIndex]) {
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

  bool value = getPropertyValue(pinIndex);
  if (!force && value==enable) return;    // already set
  
  LogHandler::logMsg(IOC_MODULE_NAME, F("MODE: "), settingsData->ioModes[pinIndex]);

  String msg = F("Set PIN ");
  msg.concat(pin);
  msg.concat(F(" to "));
  msg.concat((enable ? ENABLED_STRING : DISABLED_STRING));
  LogHandler::logMsg(IOC_MODULE_NAME, msg);
  
  digitalWrite(pin, !enable);    // map HIGH to off, LOW to on
  if (enable) {
    setPropertyValue(pinIndex, IO_STATE_ON);
  } else {
    setPropertyValue(pinIndex, IO_STATE_OFF);
  }
}

void IOController::setMode(int pinIndex, int mode) {
  settingsData->ioModes[pinIndex] = mode;
}

void IOController::addPropertyValueListener(int pinIndex, Property::ValueChangeListener *valueChangeListener) {
  switch(pinIndex) {
    case PIN_PUMP_HC_INDEX:
      this->hcState.registerValueChangeListener(valueChangeListener);
      break;
    case PIN_PUMP_WATER_INDEX:
      this->wState.registerValueChangeListener(valueChangeListener);
      break;
    case PIN_FLOW_SWITCH_INDEX:
      this->fsState.registerValueChangeListener(valueChangeListener);
      break;
  }
}

int IOController::getPropertyValue(int pinIndex) {
  switch(pinIndex) {
    case PIN_PUMP_HC_INDEX:
      return this->hcState.getValue();
    case PIN_PUMP_WATER_INDEX:
      return this->wState.getValue();
    case PIN_FLOW_SWITCH_INDEX:
      return this->fsState.getValue();
  }
}

void IOController::setPropertyValue(int pinIndex, int value) {
  switch(pinIndex) {
    case PIN_PUMP_HC_INDEX:
      this->hcState.setValue(value);
      break;
    case PIN_PUMP_WATER_INDEX:
      this->wState.setValue(value);
      break;
    case PIN_FLOW_SWITCH_INDEX:
      this->fsState.setValue(value);
      break;
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
          settingsData->ioModes[ioIndex] = v3;
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
  
  tmpStr.concat(this->hcState.getValue());
  tmpStr.concat(F(" "));
  tmpStr.concat(this->wState.getValue());
  tmpStr.concat(F(" "));
  tmpStr.concat(this->fsState.getValue());
  
  OutputHandler::sendCmd(IOC_MODULE_NAME, tmpStr);

  _printState(F("TANK PUMP"), getPropertyValue(PIN_PUMP_HC_INDEX));
  _printState(F("WATER PUMP"), getPropertyValue(PIN_PUMP_WATER_INDEX));
  _printState(F("FLOW SWITCH"), getPropertyValue(PIN_FLOW_SWITCH_INDEX));
}

void IOController::update() {
  if (settingsData->ioModes[PIN_FLOW_SWITCH_INDEX]!=PUMP_MODE_AUTO) return;
  
  int val = !digitalRead(PIN_FLOW_SWITCH);
  bool orgVal = this->fsState.getValue();
  
  if (val==orgVal) return;
  
  if (val==HIGH) {
    this->fsState.setValue(IO_STATE_ON);
//    LogHandler::logMsg(IOC_MODULE_NAME, F("FLOW SWITCH is ON"));
  } else {
//    LogHandler::logMsg(IOC_MODULE_NAME, F("FLOW SWITCH is OFF"));
    this->fsState.setValue(IO_STATE_OFF);
  }
}
