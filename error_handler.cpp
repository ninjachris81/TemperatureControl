#include "error_handler.h"

LedLogic* ErrorHandler::ledLogic = NULL;
bool ErrorHandler::hasFatalError = false;

void ErrorHandler::init(LedLogic *ledLogic) {
  ErrorHandler::ledLogic = ledLogic;
  ErrorHandler::hasFatalError = false;
}

void ErrorHandler::warning(const char *moduleName, const char* msg) {
#ifdef ERROR_PRINT
  ERROR_SERIAL.print(moduleName);
  ERROR_SERIAL.print("> WARN: ");
  ERROR_SERIAL.println(msg);
#endif
}
  
void ErrorHandler::fatal(const char *moduleName, const char* msg) {
#ifdef ERROR_PRINT
  ERROR_SERIAL.print(moduleName);
  ERROR_SERIAL.print("> FATAL: ");
  ERROR_SERIAL.println(msg);
#endif

  ErrorHandler::hasFatalError = true;
  ledLogic->setInterval(LED_ERROR_INTERVAL_MS);
}
