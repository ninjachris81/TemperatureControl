#include "temperature_logic.h"
#include "settings.h"

void TemperatureLogic::init(TempSettingsStruct &settings, IOController *ioController) {
  _updateData(true);
  this->ioController = ioController;
}

void TemperatureLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= CHECK_INTERVAL_MIN_MS) {      // last update check interval
    byte currentMinute10 = (RTC.hour * MINUTE10_FACTOR) + RTC.minute;
    if (currentMinute10 > settingsData.activeTimeStart10min && currentMinute10 < settingsData.activeTimeEnd10min) {    // in active timeframe ?
      if (_updateData(false)) {
        bool enablePump = false;
      
        if (settingsData.operatingTemp_HC<currentTemperatureHC) {
          enablePump = true;
        } else {
          if (settingsData.operatingTemp_W<currentTemperatureW) enablePump = true;
        }
        
        ioController->setValue(PIN_PUMP, PIN_PUMP_INDEX, enablePump);
      }
    }
    
    lastUpdate = millis();
  }
}

bool TemperatureLogic::_updateData(bool forceUpdate) {
  if (forceUpdate || lastTempCheck==0 || (millis() - lastTempCheck) >= CHECK_TEMP_INTERVAL_MIN_MS) {    // check sensor update interval
    tempSensorHC.read(DHT11_PIN_HC);
    currentTemperatureHC = tempSensorHC.temperature;
  
    tempSensorW.read(DHT11_PIN_W);
    currentTemperatureW = tempSensorW.temperature;
    
    lastTempCheck = millis();
    return true;
  } else {
    return false;
  }
}


