#include "display_logic.h"
#include "time_logic.h"
#include "format_utils.h"

void DisplayLogic::init() {
  display.begin(DISPLAY_I2C_ADDR);
  
  display.setLine(1, F("Initializing..."));
  display.setHorizontalLine(2);
  display.render();
  
  for (uint8_t i=0;i<logBufferSize;i++) logBuffer[i] = "";

  isFirstRender = true;
  LogHandler::registerListener(this);
}

void DisplayLogic::update() {
  if (isFirstRender) {
    if (LogHandler::hasFatalError) {
      display.setLine(1, F("Fatal init error !"));
    }
    _updateStatusLine();
    isFirstRender = false;
  } else {
    if (lastUpdate==0 || millis() - lastUpdate >= UPDATE_INTERVAL_MS) {      // last update check interval
      _updateStatusLine();
      lastUpdate = millis();
    }
  }
  
  display.render();
}

void DisplayLogic::_updateStatusLine() {
  String tmp = F("Temp Ctrl    ");
  FormatUtils::formatTime(tmp, RTC.hour, RTC.minute, RTC.second);
  display.setLine(0, tmp);
}


void DisplayLogic::onMessage(String msg, LogHandler::LOG_TYPE type) {
  String tmpMsg = "T ";
  tmpMsg.concat(millis());
  
  for (int i=0;i<logBufferSize-1;i++) {
    if (i<logBufferSize-1) logBuffer[i] = logBuffer[i+1];
    display.setLine(i + 3, logBuffer[i]);
  }
  
  logBuffer[logBufferSize-1] = tmpMsg;
  display.setLine(SSD1306Text::lineCount-1, tmpMsg);
}

