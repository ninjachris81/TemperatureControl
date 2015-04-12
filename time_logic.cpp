#include "time_logic.h"
#include "globals.h"
#include "format_utils.h"

void TimeLogic::init(){
  if (RTC.isPresent()!=0) {
    RTC.getTime();
    LogHandler::logMsg(TIME_MODULE_NAME, F("Current time: "), FormatUtils::formatTime(RTC.hour, RTC.minute, RTC.second));
    RTC.startClock();
  } else {
    LogHandler::fatal(TIME_MODULE_NAME, F("RTC not found !"));
  }
}

void TimeLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= TIME_UPDATE_INTERVAL_MS) {      // last update check interval
    if (RTC.isPresent()!=0) RTC.getTime();
    lastUpdate = millis();
  }
}

void TimeLogic::save(uint8_t hour, uint8_t minute, uint8_t second) {
  LogHandler::logMsg(TIME_MODULE_NAME, F("Saving time"));
  if (RTC.isPresent()!=0) RTC.stopClock();
  RTC.hour = hour;
  RTC.minute = minute;
  RTC.second = second;
  RTC.setTime();
  if (RTC.isPresent()!=0) RTC.startClock();
}

