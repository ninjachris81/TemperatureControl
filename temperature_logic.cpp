#include "temperature_logic.h"
#include "settings.h"

void TemperatureLogic::init(TempSettingsStruct &settings, IOController *ioController) {
  bool wasUpdated;
  
  this->settingsData = settings;
  
  if (!_updateData(true, wasUpdated)) {
    LogHandler::fatal(TEMPERATURE_MODULE_NAME, F("Failed to read from temp sensors !"));
  }
  this->ioController = ioController;
  
  /*
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Operating temp HC: "), settings.operatingTemp_HC);
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Operating temp W: "), settings.operatingTemp_W);

  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Active Time Start: "), settings.activeTimeStart10min);
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Active Time End: "), settings.activeTimeEnd10min);
  */
}

void TemperatureLogic::update() {
  bool wasUpdated;

  if (lastUpdate==0 || millis() - lastUpdate >= CHECK_INTERVAL_MIN_MS) {      // last update check interval
    byte currentMinute10 = (RTC.minute * MINUTE10_FACTOR) + RTC.minute;
    if (currentMinute10 > settingsData.activeTimeStart10min && currentMinute10 < settingsData.activeTimeEnd10min) {    // in active timeframe ?
      if (_updateData(false, wasUpdated)) {
        if (wasUpdated) {
          bool enablePump = false;
        
          if (settingsData.operatingTemp_HC<currentTemperatureHC) {
            enablePump = true;
          } else {
            if (settingsData.operatingTemp_W<currentTemperatureW) enablePump = true;
          }
          
          LogHandler::logMsg(TEMPERATURE_MODULE_NAME, (enablePump ? F("Enabling pump") : F("Disabling pump")));
          ioController->setValue(PIN_PUMP, PIN_PUMP_INDEX, enablePump);
        } else {
          LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Skipped update of temp sensor data"));
        }
      } else {
        LogHandler::warning(TEMPERATURE_MODULE_NAME, F("Failed to update data"));
      }
    } else {
      /*
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Out of time frame, start: "), settingsData.activeTimeStart10min);
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Out of time frame, end: "), settingsData.activeTimeEnd10min);
      LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Out of time frame, start current: "), RTC.minute * MINUTE10_FACTOR);
      */
    }
    
    lastUpdate = millis();
  } else {
//    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Waiting for update, lu: "), lastUpdate);
  }
}

bool TemperatureLogic::_updateData(bool forceUpdate, bool &hasUpdated) {
  hasUpdated = false;

  if (forceUpdate || lastTempCheck==0 || (millis() - lastTempCheck) >= CHECK_TEMP_INTERVAL_MIN_MS) {    // check sensor update interval
  /*
    if (!tempSensorHC.read(DHT11_PIN_HC)==DHTLIB_OK) return false;
    currentTemperatureHC = tempSensorHC.temperature;
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("New temperature HC: "), currentTemperatureHC);
    */
  
    if (!tempSensorW.read(DHT11_PIN_W)==DHTLIB_OK) return false;
    currentTemperatureW = tempSensorW.temperature;
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("New temperature W: "), currentTemperatureW);
    
    lastTempCheck = millis();
    hasUpdated = true;
  } else {
    LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Waiting for update, ltc: "), lastTempCheck);
  }
  return true;
}

void TemperatureLogic::simulateTemperature(int currentTemperatureHC, int currentTemperatureW) {
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Simulating temperature HC to "), currentTemperatureHC);
  this->currentTemperatureHC = currentTemperatureHC;
  LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Simulating temperature W to "), currentTemperatureW);
  this->currentTemperatureW = currentTemperatureW;
}

int TemperatureLogic::getTemperatureHC() {
  return this->currentTemperatureHC;
}
int TemperatureLogic::getTemperatureW() {
  return this->currentTemperatureW;
}


