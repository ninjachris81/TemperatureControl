#include "temperature_logic.h"
#include "settings.h"

void TemperatureLogic::init(TempSettingsStruct &settings, IOController *ioController) {
  bool wasUpdated;
  
  if (!_updateData(true, wasUpdated)) {
    ErrorHandler::fatal(TEMPERATURE_MODULE_NAME, F("Failed to read from temp sensors !"));
  }
  this->ioController = ioController;
}

void TemperatureLogic::update() {
  bool wasUpdated;

  if (lastUpdate==0 || millis() - lastUpdate >= CHECK_INTERVAL_MIN_MS) {      // last update check interval
    byte currentMinute10 = (RTC.hour * MINUTE10_FACTOR) + RTC.minute;
    if (currentMinute10 > settingsData.activeTimeStart10min && currentMinute10 < settingsData.activeTimeEnd10min) {    // in active timeframe ?
      if (_updateData(false, wasUpdated)) {
        if (wasUpdated) {
          bool enablePump = false;
        
          if (settingsData.operatingTemp_HC<currentTemperatureHC) {
            enablePump = true;
          } else {
            if (settingsData.operatingTemp_W<currentTemperatureW) enablePump = true;
          }
          
          ioController->setValue(PIN_PUMP, PIN_PUMP_INDEX, enablePump);
        } else {
          LogHandler::logMsg(TEMPERATURE_MODULE_NAME, F("Skipped update of temp sensor data"));
        }
      }
    }
    
    lastUpdate = millis();
  }
}

bool TemperatureLogic::_updateData(bool forceUpdate, bool &hasUpdated) {
  hasUpdated = false;

  if (forceUpdate || lastTempCheck==0 || (millis() - lastTempCheck) >= CHECK_TEMP_INTERVAL_MIN_MS) {    // check sensor update interval
    if (!tempSensorHC.read(DHT11_PIN_HC)==DHTLIB_OK) return false;
    currentTemperatureHC = tempSensorHC.temperature;
  
    if (!tempSensorW.read(DHT11_PIN_W)==DHTLIB_OK) return false;
    currentTemperatureW = tempSensorW.temperature;
    
    lastTempCheck = millis();
    hasUpdated = true;
  }
  return true;
}


