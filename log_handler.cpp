#include "log_handler.h"

void LogHandler::init() {
  DEBUG_SERIAL.begin(9600);
}

void LogHandler::logMsg(const char *moduleName, const char* msg) {
#ifdef DEBUG_PRINT
  DEBUG_SERIAL.print(moduleName);
  DEBUG_SERIAL.print("> ");
  DEBUG_SERIAL.println(msg);
#endif
}

void LogHandler::logMsg(const char *moduleName, String msg) {
#ifdef DEBUG_PRINT
  char buf[msg.length() + 1];
  msg[msg.length()] = 0;
  
  msg.toCharArray(buf, msg.length() + 1);
  
  logMsg(moduleName, buf);
#endif
}

