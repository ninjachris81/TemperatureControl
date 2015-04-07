#include "log_handler.h"

void LogHandler::init() {
  DEBUG_SERIAL.begin(9600);
}

void LogHandler::logMsg(const char *moduleName, String msg) {
#ifdef DEBUG_PRINT
  DEBUG_SERIAL.print(moduleName);
  DEBUG_SERIAL.print(F("> "));
  DEBUG_SERIAL.println(msg);
#endif
}

void LogHandler::logMsg(const char *moduleName, String msg, int val) {
#ifdef DEBUG_PRINT
  DEBUG_SERIAL.print(moduleName);
  DEBUG_SERIAL.print(F("> "));
  DEBUG_SERIAL.print(msg);
  DEBUG_SERIAL.println(val, DEC);
#endif
}

void LogHandler::logMsg(const char *moduleName, String msg, String val) {
#ifdef DEBUG_PRINT
  DEBUG_SERIAL.print(moduleName);
  DEBUG_SERIAL.print(F("> "));
  DEBUG_SERIAL.print(msg);
  DEBUG_SERIAL.println(val);
#endif
}
