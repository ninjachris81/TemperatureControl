#ifndef time_logic_h
#define time_logic_h

#include "DS1307new.h"
#include "input_handler.h"

#define TIME_MODULE_NAME "TIME"

#define TIME_UPDATE_INTERVAL_MS 1000

class TimeLogic : public InputHandler::InputListener {
public:
  
  void init();
  void update();
  
  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);

  static void save(uint8_t hour, uint8_t minute, uint8_t second);
  
private:
  unsigned long lastUpdate = 0;
};

#endif

