#include <SoftwareSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "input_handler.h"
#include "globals.h"

class SerialApi {
public:
  void init();
  
  void update();

private:  
  String tmpBuffer;

};
