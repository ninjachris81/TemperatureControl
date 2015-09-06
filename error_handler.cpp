#include "error_handler.h"

uint8_t ErrorHandler::fatalErrors = 0;

void ErrorHandler::increaseFatalErrors() {
  ErrorHandler::fatalErrors++;
}

void ErrorHandler::clearFatalErrors() {
  ErrorHandler::fatalErrors = 0;
}

bool ErrorHandler::hasFatalErrors() {
  return ErrorHandler::fatalErrors > 0;
}

