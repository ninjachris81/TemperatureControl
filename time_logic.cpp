#include "time_logic.h"
#include "globals.h"
#include "format_utils.h"
#include "output_handler.h"
#include "log_handler.h"

void TimeLogic::init(){
  if (RTC.isPresent()!=0) {
    RTC.getTime();
    LogHandler::logMsg(TIME_MODULE_NAME, F("Time: "), FormatUtils::formatTime(RTC.hour, RTC.minute, RTC.second));
    RTC.startClock();
  } else {
    LogHandler::fatal(TIME_MODULE_NAME, F("RTC not found"));
  }
  
  InputHandler::registerListener(this);
}

void TimeLogic::update() {
  if (lastUpdate==0 || millis() - lastUpdate >= TIME_UPDATE_INTERVAL_MS) {      // last update check interval
    if (RTC.isPresent()!=0) RTC.getTime();
    lastUpdate = millis();
  }
}

String TimeLogic::getName() {
  return TIME_MODULE_NAME;
}

bool TimeLogic::onInput(String cmd) {
  if (cmd.equals(F("GET"))) {
    String tmpStr = F("CT ");
    tmpStr.concat((RTC.hour * 24 * 60) + (RTC.minute * 60) + RTC.second);
    OutputHandler::sendCmd(TIME_MODULE_NAME, tmpStr);
    LogHandler::logMsg(TIME_MODULE_NAME, F("Time: "), FormatUtils::formatTime(RTC.hour, RTC.minute, RTC.second));
    return true;
  } else if (cmd.startsWith(F("SET"))) {
    int v1, v2, v3;
    if (InputHandler::parseParameters3(cmd.substring(4), v1, v2, v3)) {
      // set time
      save(v1, v2, v3);
    } else {
      LogHandler::warning(TIME_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
    }        
    return true;
  }
  
  return false;
}


void TimeLogic::save(uint8_t hour, uint8_t minute, uint8_t second) {
  LogHandler::logMsg(TIME_MODULE_NAME, F("Saving: "), FormatUtils::formatTime(hour, minute, second));
  if (RTC.isPresent()!=0) RTC.stopClock();
  RTC.hour = hour;
  RTC.minute = minute;
  RTC.second = second;
  RTC.setTime();
  if (RTC.isPresent()!=0) RTC.startClock();
}

