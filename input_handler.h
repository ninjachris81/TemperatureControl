#ifndef input_handler_h
#define input_handler_h

#include "globals.h"
#include "settings.h"
#include "temperature_logic.h"
#include "time_logic.h"

#define INPUT_HANDLER_MODULE_NAME F("InputHandler")

class InputHandler {
public:
  void init(Settings* settings, TemperatureLogic* temperatureLogic, TimeLogic* timeLogic);
  
  void update();
  
private:
  String tmpBuffer;
  
  Settings* settings;
  TemperatureLogic* temperatureLogic;
  TimeLogic* timeLogic;
  
  bool parseParameters2(String &bufferStr, int index, int &v1, int &v2);

  bool parseParameters3(String &bufferStr, int index, int &v1, int &v2, int &v3);

};


#endif
