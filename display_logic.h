#ifndef display_logic_h
#define display_logic_h

#include "ssd1306_text.h"

#define DISPLAY_I2C_ADDR 0x3C

class DisplayLogic {
public:
  void init();
  
  void update();
  
private:
  SSD1306Text display;
  
  bool isFirstRender;

};

#endif
