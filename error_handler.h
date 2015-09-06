#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class ErrorHandler {
public:

  static void increaseFatalErrors();
  static void clearFatalErrors();
  static bool hasFatalErrors();

  static uint8_t fatalErrors;

};
