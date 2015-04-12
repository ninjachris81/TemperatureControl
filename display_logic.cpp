#include "display_logic.h"
#include "error_handler.h"
#include "time_logic.h"
#include "format_utils.h"

void DisplayLogic::init() {
  display.begin(DISPLAY_I2C_ADDR);
  
  display.setLine(0, F("Temperature Control"));
  display.setLine(1, F("Initializing..."));
  display.render();
  
  isFirstRender = true;
}

void DisplayLogic::update() {
  if (isFirstRender) {
    
    //if (ErrorHandler::hasFatalError) {
    //  display.setLine(1, F("Fatal init error !"));
    //} else {
      String tmp = F("        ");
      FormatUtils::formatTime(tmp, RTC.hour, RTC.minute, RTC.second);
      display.setLine(7, tmp);
    //}
    
    isFirstRender = false;
  }
  
  display.render();
}
