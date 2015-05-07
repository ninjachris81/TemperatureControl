#ifndef display_logic_h
#define display_logic_h

#include "ssd1306_text.h"
#include "log_handler.h"
#include "joystick_handler.h"
#include "input_handler.h"

#define DISPLAY_I2C_ADDR 0x3C

#define UPDATE_INTERVAL_MS 1000

#define DISPLAY_HANDLER_MODULE_NAME F("DISP")

#define MENU_1_1 F("Settings")
#define MENU_1_2 F("Monitor")

class DisplayLogic : public LogHandler::LogListener, public JoystickHandler::JoystickFeedbackHandler, public InputHandler::InputListener {
public:
  void init();
  
  void update();
  
  /*virtual*/ void onMessage(String msg, LogHandler::LOG_TYPE type=LogHandler::LOG);
  
  /*virtual*/ void onKeyEvent(JoystickHandler::JoystickFeedbackHandler::KEY_TYPE type, bool isDown);
  
  /*virtual*/ String getName();
  
  /*virtual*/ bool onInput(String cmd);

private:
  static const uint8_t logBufferSize = SSD1306Text::lineCount - 4;    // 2 lines statusbar, 1 line spacer, 1 is not needed (last line discharded)
  String logBuffer[logBufferSize];    

  unsigned long lastUpdate = 0;
  
  SSD1306Text display;
  
  bool isFirstRender;
  
  void _updateStatusLine();
  
  JoystickHandler jh;
  
  uint8_t currentMenuLine = 4;
  
  void _refreshMenu();
  
  void setMenuLine(uint8_t index, String menuName);
  
  void simulateKey(uint8_t type, bool isDown); 

};

#endif
