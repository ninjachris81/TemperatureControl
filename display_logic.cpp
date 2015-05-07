#include "display_logic.h"
#include "time_logic.h"
#include "format_utils.h"

void DisplayLogic::init() {
  display.begin(DISPLAY_I2C_ADDR);
  
  display.setLine(1, F("Initializing..."));
  display.setHorizontalLine(2);
  display.render();
  
  //for (uint8_t i=0;i<logBufferSize;i++) logBuffer[i] = "";

  isFirstRender = true;
//  LogHandler::registerListener(this);
  jh.setFeedbackHandler(this);
  jh.init();
  
  InputHandler::registerListener(this);
}

void DisplayLogic::update() {
  if (isFirstRender) {
    if (LogHandler::hasFatalError) {
      display.setLine(1, F("Fatal init error !"));
    }
    _updateStatusLine();
    currentMenuLine = 4;
    _refreshMenu();
    isFirstRender = false;
  } else {
    if (lastUpdate==0 || millis() - lastUpdate >= UPDATE_INTERVAL_MS) {      // last update check interval
      _updateStatusLine();
      lastUpdate = millis();
    }
  }
  
  jh.update();
  
  display.render();
}

String DisplayLogic::getName() {
  return DISPLAY_HANDLER_MODULE_NAME;
}
  
bool DisplayLogic::onInput(String cmd) {
  if (cmd.equals(F("KEY"))) {
    int v1, v2;
    if (InputHandler::parseParameters2(cmd.substring(4), v1, v2)) {
      // set temp
      simulateKey(v1, v2);
    } else {
      LogHandler::logMsg(DISPLAY_HANDLER_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
    }
    return true;
  }
  
  return false;
}

void DisplayLogic::_updateStatusLine() {
  String tmp = F("Temp Ctrl    ");
  FormatUtils::formatTime(tmp, RTC.hour, RTC.minute, RTC.second);
  display.setLine(0, tmp);
}


void DisplayLogic::onMessage(String msg, LogHandler::LOG_TYPE type) {
  String tmpMsg = F("T ");
  tmpMsg.concat(millis());
  
  for (int i=0;i<logBufferSize-1;i++) {
    if (i<logBufferSize-1) logBuffer[i] = logBuffer[i+1];
    display.setLine(i + 3, logBuffer[i]);
  }
  
  logBuffer[logBufferSize-1] = tmpMsg;
  display.setLine(SSD1306Text::lineCount-1, tmpMsg);
}

void DisplayLogic::simulateKey(uint8_t type, bool isDown) {
  onKeyEvent((JoystickHandler::JoystickFeedbackHandler::KEY_TYPE)type, isDown);
}

void DisplayLogic::onKeyEvent(JoystickHandler::JoystickFeedbackHandler::KEY_TYPE type, bool isDown) {
  if (!isDown) return;
  if (type==KEY_DOWN) currentMenuLine++;
  if (type==KEY_UP) currentMenuLine--;
  _refreshMenu();
}

void DisplayLogic::_refreshMenu() {
  setMenuLine(4, F("Settings"));
  setMenuLine(5, F("Monitor"));
  setMenuLine(6, F("Log Trace"));
}

void DisplayLogic::setMenuLine(uint8_t index, String menuName) {
  String tmp = currentMenuLine==index ? "> " : "";
  tmp.concat(menuName);
  display.setLine(index, tmp);
}

