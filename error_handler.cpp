#include "error_handler.h"

LedLogic* ErrorHandler::ledLogic = NULL;
bool ErrorHandler::hasFatalError = false;

void ErrorHandler::init(LedLogic *ledLogic) {
  ErrorHandler::ledLogic = ledLogic;
  ErrorHandler::hasFatalError = false;
}

void ErrorHandler::warning(const char *moduleName, String msg, int val) {
#ifdef ERROR_PRINT
  ERROR_SERIAL.print(moduleName);
  ERROR_SERIAL.print(F("> WARN: "));
  ERROR_SERIAL.print(msg);
  ERROR_SERIAL.println(val, DEC);
#endif

}

void ErrorHandler::fatal(const char *moduleName, String msg) {
#ifdef ERROR_PRINT
  ERROR_SERIAL.print(moduleName);
  ERROR_SERIAL.print(F("> FATAL: "));
  ERROR_SERIAL.println(msg);
#endif

  ErrorHandler::hasFatalError = true;
  ledLogic->setInterval(LED_ERROR_INTERVAL_MS);
}
