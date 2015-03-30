#ifndef error_handler_h
#define error_handler_h

#include "led_logic.h"

#define ERROR_PRINT
#define ERROR_SERIAL Serial

#define LED_ERROR_INTERVAL_MS 200

class ErrorHandler {
public:
  static void init(LedLogic *ledLogic);
  
  static void warning(const char *moduleName, const char* msg);
  
  static void fatal(const char *moduleName, const char* msg);
  
  static LedLogic *ledLogic;
  
  static bool hasFatalError;
};

#endif
