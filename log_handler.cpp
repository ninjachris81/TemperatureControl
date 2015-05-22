#include "log_handler.h"

LogHandler::LogListener* LogHandler::_listeners[MAX_LOG_LISTENERS];
uint8_t LogHandler::listenerCount = 0;
LedLogic* LogHandler::ledLogic = NULL;
bool LogHandler::hasFatalError = false;

bool LogHandler::registerListener(LogHandler::LogListener* listener) {
  if (listenerCount>=MAX_LOG_LISTENERS) return false;
  _listeners[listenerCount] = listener;
  listener->setIndex(listenerCount);
  listenerCount++;
  return true;
}

void LogHandler::unregisterListener(LogListener* listener) {
  uint8_t index = listener->getIndex();
  delete _listeners[index];
  _listeners[index] = NULL;
  listenerCount--;
}

void LogHandler::init(LedLogic *ledLogic) {
  LogHandler::ledLogic = ledLogic;
  
  DEBUG_SERIAL.begin(9600);
  
  for (uint8_t i=0;i<MAX_LOG_LISTENERS;i++) {
    _listeners[i]==NULL;
  }
}

void LogHandler::logMsg(String moduleName, String msg) {
  String str = moduleName;
  str.concat(F("> "));
  str.concat(msg);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str);
}

void LogHandler::logMsg(String moduleName, String msg, int val) {
  String str = moduleName;
  str.concat(F("> "));
  str.concat(msg);
  str.concat(val);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str);
}

void LogHandler::logMsg(String moduleName, String msg, String val) {
  String str = moduleName;
  str.concat(F("> "));
  str.concat(msg);
  str.concat(val);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str);
}

void LogHandler::warning(String moduleName, String msg) {
  String str = moduleName;
  str.concat(F("> WARN: "));
  str.concat(msg);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str, WARN);
}

void LogHandler::warning(String moduleName, String msg, int val) {
  String str = moduleName;
  str.concat(F("> WARN: "));
  str.concat(msg);
  str.concat(val);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str, WARN);
}

void LogHandler::warning(String moduleName, String msg, String val) {
  String str = moduleName;
  str.concat(F("> WARN: "));
  str.concat(msg);
  str.concat(val);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str, WARN);
}

void LogHandler::fatal(String moduleName, String msg) {
  String str = moduleName;
  str.concat(F("> FATAL: "));
  str.concat(msg);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  LogHandler::hasFatalError = true;
  ledLogic->setInterval(LED_ERROR_INTERVAL_MS);
  
  sendToListeners(str, FATAL);
}

void LogHandler::sendToListeners(String msg, LogHandler::LOG_TYPE type) {
  if (listenerCount>0) {
    for (uint8_t i=0;i<MAX_LOG_LISTENERS;i++) {
      if (_listeners[i]!=NULL) {
        _listeners[i]->onLogMessage(msg, type);
      }
    }
  }
}

