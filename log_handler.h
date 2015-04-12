#ifndef log_handler_h
#define log_handler_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DEBUG_PRINT
#define DEBUG_SERIAL Serial

class LogHandler {
  public:
  
  static void init();

  static void logMsg(String moduleName, String msg);

  static void logMsg(String moduleName, String msg, int val);

  static void logMsg(String moduleName, String msg, String val);

};

#endif
