#ifndef display_logic_h
#define display_logic_h

#include "ssd1306_text.h"
#include "log_handler.h"

#define DISPLAY_I2C_ADDR 0x3C

#define UPDATE_INTERVAL_MS 1000

class DisplayLogic : public LogHandler::LogListener {
public:
  void init();
  
  void update();
  
  /*virtual*/ void onMessage(String msg, LogHandler::LOG_TYPE type=LogHandler::LOG);
  
private:
  static const uint8_t logBufferSize = SSD1306Text::lineCount - 4;    // 2 lines statusbar, 1 line spacer, 1 is not needed (last line discharded)
  String logBuffer[logBufferSize];    

  unsigned long lastUpdate = 0;
  
  SSD1306Text display;
  
  bool isFirstRender;
  
  void _updateStatusLine();

};

#endif
