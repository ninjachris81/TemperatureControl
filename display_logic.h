#ifndef display_logic_h
#define display_logic_h

#include "ssd1306_text.h"

class DisplayLogic {
public:
  void init();
  
  void update();
  
private:
  SSD1306Text display;
  bool needsRefresh;
  
  bool isFirstRender;

};

#endif
