#include "temperature_logic.h"
#include "settings.h"
#include "output_handler.h"

TemperatureLogic::TemperatureLogic() {
  this->wire = new OneWire(PIN_TEMP);
  this->tempSensors = new DallasTemperature(wire);
}

TemperatureLogic::~TemperatureLogic() {
  delete wire;
  delete tempSensors;
}

void TemperatureLogic::init(TempSettingsStruct &settings, IOController *ioController) {
  bool wasUpdated;
  
  this->settingsData = settings;
  this->ioController = ioController;
  
  tempSensors->begin();
  byte addr[8];

  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Searching temperature sensors..."));
  while(wire->search(addr)) {
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Found one temperature sensor"));
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("CRC is not valid for temp sensor !"));
    }
  }
  wire->reset_search();

  _updateData(true, wasUpdated);
  if (!wasUpdated) {
    LogHandler::fatal(TEMPERATURE_MODULE_NAME, F("Failed to read from temp sensors !"));
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

String TemperatureLogic::getName() {
  return TEMPERATURE_MODULE_NAME;
}

bool TemperatureLogic::onInput(String cmd) {
  if (cmd.equals(F("GET"))) {
    // read temp
    
    String tmpCmd = F("CT ");
    tmpCmd.concat(currentTemperatureHC);
    tmpCmd.concat(F(" "));
    tmpCmd.concat(currentTemperatureW);
    
    OutputHandler::sendCmd(TEMPERATURE_MODULE_NAME, tmpCmd);
    
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("HC temperature is "), currentTemperatureHC);
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("W temperature is "), currentTemperatureW);
    return true;
  } else if (cmd.startsWith(F("SET"))) {
    int v1, v2;
    if (InputHandler::parseParameters2(cmd, v1, v2)) {
      // set temp
      simulateTemperature(v1, v2);
    } else {
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
    }        
    return true;
  }
  
  return false;
}

void TemperatureLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= CHECK_INTERVAL_MIN_MS) {      // last update check interval
    _updateData(false);    // update temp sensors

    bool enablePumpW = false;
    bool enablePumpHC = false;
    
    checkDefault(enablePumpW, enablePumpHC);
    if (!enablePumpW || !enablePumpHC) checkPreheating(enablePumpW, enablePumpHC);

//    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("PumpW: "), enablePumpW ? ENABLED_STRING : DISABLED_STRING);
//    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("PumpHC: "), enablePumpHC ? ENABLED_STRING : DISABLED_STRING);
    
    ioController->setValue(PIN_PUMP_WATER, PIN_PUMP_WATER_INDEX, enablePumpW);
    ioController->setValue(PIN_PUMP_HC, PIN_PUMP_HC_INDEX, enablePumpHC);

    lastUpdate = millis();
  } else {
//    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Waiting for update, lu: "), lastUpdate);
  }
}

void TemperatureLogic::checkDefault(bool &enablePumpW, bool &enablePumpHC) {
    if (ioController->getValue(PIN_FLOW_SWITCH_INDEX)) {
      // check if flow switch is on
//      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Flow Switch is On"));
      enablePumpW = true;
      enablePumpHC = true;
    } else {
      // check maintain temp mode
      byte currentMinute10 = (RTC.hour * MINUTE10_FACTOR) + (RTC.minute / (60/MINUTE10_FACTOR));
      if (currentMinute10 >= settingsData.operatingStart10Minutes && currentMinute10 < settingsData.operatingEnd10Minutes) {
        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain temp mode active"));
        enablePumpW = currentTemperatureW<settingsData.operatingTempMin_W;
        enablePumpHC = currentTemperatureHC<settingsData.operatingTempMin_HC;
//        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain temp w min: "), settingsData.operatingTempMin_W);
//        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain temp hc min: "), settingsData.operatingTempMin_HC);
      } else {
        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain - Out of time frame, start: "), settingsData.operatingStart10Minutes);
        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain - Out of time frame, end: "), settingsData.operatingEnd10Minutes);
        LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Maintain - Out of time frame, current: "), currentMinute10);
      }
    }
}

void TemperatureLogic::checkPreheating(bool &enablePumpW, bool &enablePumpHC) {
    int currentMinute = (RTC.hour * 60) + RTC.minute;
    int currentPreheatingStart = settingsData.preheatingStart10Minutes * MINUTE10_FACTOR;
    int currentPreheatingEnd = currentPreheatingStart + settingsData.preheatingDurationMinutes;
    
    if (currentMinute >= currentPreheatingStart && currentMinute < currentPreheatingEnd) {    // in active timeframe ?
      enablePumpHC = settingsData.preheatingTempMin_HC<currentTemperatureHC;
      enablePumpW = settingsData.preheatingTempMin_W<currentTemperatureW;
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Preheating mode active, current HC temp: "), currentTemperatureHC);
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Preheating mode active, current W temp: "), currentTemperatureW);
    } else {
      /*
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Preheating - Out of time frame, start: "), currentPreheatingStart);
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Preheating - Out of time frame, end: "), currentPreheatingEnd);
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Preheating - Out of time frame, current: "), currentMinute);
      */
    }
}

void TemperatureLogic::_updateData(bool forceUpdate) {
  bool wasUpdated;
  _updateData(forceUpdate, wasUpdated);
}

void TemperatureLogic::_updateData(bool forceUpdate, bool &hasUpdated) {
  hasUpdated = false;

  if (forceUpdate || lastTempCheck==0 || (millis() - lastTempCheck) >= CHECK_TEMP_INTERVAL_MIN_MS) {    // check sensor update interval
    tempSensors->requestTemperatures();
    //if (tempSensors->getDeviceCount()!=2) return false;
    
    currentTemperatureHC = tempSensors->getTempCByIndex(TEMP_INDEX_HC);
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("New temperature HC: "), currentTemperatureHC);
  
    currentTemperatureW = tempSensors->getTempCByIndex(TEMP_INDEX_W);
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("New temperature W: "), currentTemperatureW);
    
    lastTempCheck = millis();
    hasUpdated = true;
  } else {
    //LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Waiting for update, ltc: "), lastTempCheck);
  }
}

void TemperatureLogic::simulateTemperature(int currentTemperatureHC, int currentTemperatureW) {
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Simulating temperature HC to "), currentTemperatureHC);
  this->currentTemperatureHC = currentTemperatureHC;
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Simulating temperature W to "), currentTemperatureW);
  this->currentTemperatureW = currentTemperatureW;
}


