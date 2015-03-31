#include "time_logic.h"
#include "globals.h"

void TimeLogic::init(){
  if (RTC.isPresent()!=0) {
    RTC.getTime();
    LogHandler::logMsg(TIME_MODULE_NAME, "Current time: " + RTC.hour);
  } else {
    ErrorHandler::fatal(TIME_MODULE_NAME, F("RTC not found !"));
  }
}

void TimeLogic::update() {
}

