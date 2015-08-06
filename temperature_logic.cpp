#include "temperature_logic.h"
#include "settings.h"
#include "output_handler.h"
#include "log_handler.h"

TemperatureLogic::TemperatureLogic() {
  this->wire = new OneWire(PIN_TEMP);
  this->tempSensors = new DallasTemperature(wire);
  simulateTemp = false;

  this->tempHC.init(TEMP_INDEX_HC);
  this->tempW.init(TEMP_INDEX_W);
  this->tempW.init(TEMP_INDEX_TANK);

  this->tempHC.registerValueChangeListener(this);
  this->tempW.registerValueChangeListener(this);
  //this->tempTank.registerValueChangeListener(this);
}

TemperatureLogic::~TemperatureLogic() {
  delete wire;
  delete tempSensors;
}

void TemperatureLogic::init(TempSettingsStruct *settings, IOController *ioController) {
  bool wasUpdated;
  
  this->settingsData = settings;
  this->ioController = ioController;
  
  tempSensors->begin();
  byte addr[8];

  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Search temp sensors"));
  while(wire->search(addr)) {
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Found temp sensor"));
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      LogHandler::warning(TEMPERATURE_MODULE_NAME, F("CRC error"));
    }
  }
  wire->reset_search();

  bool wasUpdatedW;
  bool wasUpdatedHC;
  bool wasUpdatedTank;
  _updateData(true, wasUpdatedW, wasUpdatedHC, wasUpdatedTank);
  if (!wasUpdatedHC || !wasUpdatedW) {
    LogHandler::fatal(TEMPERATURE_MODULE_NAME, F("Failed to read from sensors"));
  }
  
  /*
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Operating temp HC: "), settings.operatingTemp_HC);
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Operating temp W: "), settings.operatingTemp_W);

  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Active Time Start: "), settings.activeTimeStart10min);
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Active Time End: "), settings.activeTimeEnd10min);
  */
  
//  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Number of temp sensors found: "), tempSensors->getDeviceCount());
  InputHandler::registerListener(this);
}

void TemperatureLogic::onPropertyValueChange(uint8_t id, int value) {
  sendCurrentTemp();
}

String TemperatureLogic::getName() {
  return TEMPERATURE_MODULE_NAME;
}

bool TemperatureLogic::onInput(String cmd) {
  if (cmd.equals(F("GET"))) {
    // read temp
    sendCurrentTemp();
    return true;
  } else {
    String tempCmd;
    int v1, v2;
    if (InputHandler::parseParameters3(cmd, tempCmd, v1, v2)) {
      if (tempCmd.equals(F("SET"))) {
        // set temp
        simulateTemperature(v1, v2);
      }
    } else {
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
    }        
    return true;
  }
  
  return false;
}

void TemperatureLogic::sendCurrentTemp() {
  String tmpCmd = F("CT ");
  tmpCmd.concat(tempHC.getValue());
  tmpCmd.concat(F(" "));
  tmpCmd.concat(tempW.getValue());
  
  OutputHandler::sendCmd(TEMPERATURE_MODULE_NAME, tmpCmd);
  
  //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("HC temperature is "), currentTemperatureHC);
  //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("W temperature is "), currentTemperatureW);
}

void TemperatureLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= CHECK_INTERVAL_MIN_MS) {      // last update check interval
    bool wasUpdatedW;
    bool wasUpdatedHC;
    bool wasUpdatedTank;
  
    _updateData(false, wasUpdatedW, wasUpdatedHC, wasUpdatedTank);    // update temp sensors
    
    //if (wasUpdatedW || wasUpdatedHC) sendCurrentTemp();

    bool enablePumpW = false;
    bool enablePumpHC = false;
    
    checkDefault(enablePumpW, enablePumpHC);
    if (!enablePumpW || !enablePumpHC) checkPreheating(enablePumpW, enablePumpHC);

    //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("PumpW: "), enablePumpW ? ENABLED_STRING : DISABLED_STRING);
    //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("PumpHC: "), enablePumpHC ? ENABLED_STRING : DISABLED_STRING);
    
    ioController->setValue(PIN_PUMP_WATER, PIN_PUMP_WATER_INDEX, enablePumpW);
    ioController->setValue(PIN_PUMP_HC, PIN_PUMP_HC_INDEX, enablePumpHC);

    lastUpdate = millis();
  } else {
//    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Waiting for update, lu: "), lastUpdate);
  }
}

void TemperatureLogic::checkDefault(bool &enablePumpW, bool &enablePumpHC) {
    if (ioController->getPropertyValue(PIN_FLOW_SWITCH_INDEX)==IO_STATE_ON) {
      // check if flow switch is on
//      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Flow Switch is On"));
      enablePumpW = true;
      enablePumpHC = true;
    } else {
      // check maintain temp mode
      byte currentMinute10 = (RTC.hour * MINUTE10_FACTOR) + (RTC.minute / (60/MINUTE10_FACTOR));
      if (currentMinute10 >= settingsData->operatingStart10Minutes && currentMinute10 < settingsData->operatingEnd10Minutes) {
        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain"));
        enablePumpW = tempW.getValue()<settingsData->operatingTempMin_W;
        enablePumpHC = tempHC.getValue()<settingsData->operatingTempMin_HC;
//        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain temp w min: "), settingsData.operatingTempMin_W);
//        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain temp hc min: "), settingsData.operatingTempMin_HC);
      } else {
        //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain-Out tf, start: "), settingsData.operatingStart10Minutes);
        //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain-Out tf, end: "), settingsData.operatingEnd10Minutes);
        //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain-Out tf, current: "), currentMinute10);
      }
    }
}

void TemperatureLogic::checkPreheating(bool &enablePumpW, bool &enablePumpHC) {
    int currentMinute = (RTC.hour * 60) + RTC.minute;
    int currentPreheatingStart = settingsData->preheatingStart10Minutes * MINUTE10_FACTOR;
    int currentPreheatingEnd = currentPreheatingStart + settingsData->preheatingDurationMinutes;
    
    if (currentMinute >= currentPreheatingStart && currentMinute < currentPreheatingEnd) {    // in active timeframe ?
      enablePumpHC = settingsData->preheatingTempMin_HC<tempHC.getValue();
      enablePumpW = settingsData->preheatingTempMin_W<tempW.getValue();
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("PreH,HC temp: "), tempHC.getValue());
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("PreH,W temp: "), tempW.getValue());
    } else {
      /*
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Preheating - Out of time frame, start: "), currentPreheatingStart);
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Preheating - Out of time frame, end: "), currentPreheatingEnd);
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Preheating - Out of time frame, current: "), currentMinute);
      */
    }
}

void TemperatureLogic::_updateData(bool forceUpdate) {
  bool wasUpdatedW;
  bool wasUpdatedHC;
  bool wasUpdatedTank;
  _updateData(forceUpdate, wasUpdatedW, wasUpdatedHC, wasUpdatedTank);
}

void TemperatureLogic::_updateData(bool forceUpdate, bool &hasUpdatedW, bool &hasUpdatedHC, bool &hasUpdatedTank) {
  hasUpdatedW = false;
  hasUpdatedHC = false;
  
  if (simulateTemp) {
    hasUpdatedW = true;
    hasUpdatedHC = true;
    return;
  }
  
  if (forceUpdate || lastTempCheck==0 || (millis() - lastTempCheck) >= CHECK_TEMP_INTERVAL_MIN_MS) {    // check sensor update interval
    tempSensors->requestTemperatures();
    //if (tempSensors->getDeviceCount()!=2) return false;
    
    int temp = 0;
    
    temp = tempSensors->getTempCByIndex(TEMP_INDEX_HC);
    hasUpdatedHC = this->tempHC.setValue(temp);
    if (hasUpdatedHC) {
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Temp HC: "), tempHC.getValue());
    }
  
    temp = tempSensors->getTempCByIndex(TEMP_INDEX_W);
    hasUpdatedW = this->tempW.setValue(temp);
    if (hasUpdatedW) {
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Temp W: "), tempW.getValue());
    }
    
    temp = tempSensors->getTempCByIndex(TEMP_INDEX_TANK);
    hasUpdatedTank = this->tempTank.setValue(temp);
    if (hasUpdatedTank) {
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Temp Tank: "), tempTank.getValue());
    }

    lastTempCheck = millis();
  } else {
    //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Waiting for update, ltc: "), lastTempCheck);
  }
}

void TemperatureLogic::simulateTemperature(int temperatureW, int temperatureHC) {
  if (temperatureW==999&&temperatureHC==999) {
    simulateTemp = false;
  } else {
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Simu temp W "), temperatureW);
    this->tempW.setValue(temperatureW);
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Simu temp HC "), temperatureHC);
    this->tempHC.setValue(temperatureHC);
    simulateTemp = true;
  }
}

int TemperatureLogic::getCurrentTemperatureHC() {
  return this->tempHC.getValue();
}

int TemperatureLogic::getCurrentTemperatureW() {
  return this->tempW.getValue();
}

int TemperatureLogic::getCurrentTemperatureTank() {
  return this->tempTank.getValue();
}

