#include "errordetector_logic.h"
#include "log_handler.h"

void ErrorDetectorLogic::init(IOController *ioController, WifiLogic *wifiLogic, TemperatureLogic *tempLogic) {
  this->ioController = ioController;
  this->wifiLogic = wifiLogic;
  this->tempLogic = tempLogic;

  for (uint8_t i=0;i<ERROR_COUNT;i++) {
    durations[i] = 0;
    errors[i] = false;
  }
  
  max_durations[ERROR_INDEX_HC] = ERROR_TIMEOUT_HC_MS;
  max_durations[ERROR_INDEX_TEMP_WATER] = ERROR_TIMEOUT_TEMP_MS;
  max_durations[ERROR_INDEX_TEMP_HC] = ERROR_TIMEOUT_TEMP_MS;
}

void ErrorDetectorLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= ERROR_CHECK_INTERVAL) {
    checkErrors();
    lastUpdate = millis();
  }
}

void ErrorDetectorLogic::checkErrors() {

  checkError(ioController->getPropertyValue(PIN_PUMP_HC_INDEX)==IO_STATE_ON, PIN_PUMP_HC_INDEX);
  checkError(tempLogic->getCurrentTemperatureW()==0, ERROR_INDEX_TEMP_WATER);
  checkError(tempLogic->getCurrentTemperatureHC()==0, ERROR_INDEX_TEMP_HC);
  
  for (uint8_t i=0;i<ERROR_COUNT;i++) {
    if (!errors[i] && durations[i]>max_durations[i]) {
      // ok, we have a problem
      reportError(i);
      errors[i] = true;
    }
  }
}

void ErrorDetectorLogic::checkError(bool hasError, uint8_t index) {
  if (hasError) {
    durations[index] += ERROR_CHECK_INTERVAL;
  } else {
    durations[index] = 0;
    errors[index] = false;
  }
}


void ErrorDetectorLogic::reportError(uint8_t type) {
  switch(type) {
    case ERROR_INDEX_HC:
      this->wifiLogic->reportError(F("HEAT_CHANGER_PUMP"));
      break;
  }
}

