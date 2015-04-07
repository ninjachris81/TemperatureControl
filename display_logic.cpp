#include "display_logic.h"
#include "error_handler.h"
#include "time_logic.h"

void DisplayLogic::init() {
  display.begin(0x3C);
  
  display.setLine(0, F("Temperature Control"));
  display.setLine(1, F("Initializing..."));
  display.render();
  
  needsRefresh = false;
  isFirstRender = true;
}

void DisplayLogic::update() {
  if (isFirstRender) {
    
    //if (ErrorHandler::hasFatalError) {
    //  display.setLine(1, F("Fatal init error !"));
    //} else {
      String tmp = F("        ");
      tmp.concat(RTC.hour);
      tmp.concat(F(":"));
      tmp.concat(RTC.minute);
      tmp.concat(F(":"));
      tmp.concat(RTC.second);
      display.setLine(7, tmp);
    //}
    
    needsRefresh = true;
    isFirstRender = false;
  }
  
  if (needsRefresh) display.render();
}
