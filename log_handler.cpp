#include "log_handler.h"

LogHandler::LogListener* LogHandler::_listeners[MAX_LOG_LISTENERS];
uint8_t LogHandler::listenerCount = 0;
bool LogHandler::doLog = true;
LogHandler* LogHandler::logHandler = NULL;
  
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

void LogHandler::init() {
  DEBUG_SERIAL.begin(9600);
  
  for (uint8_t i=0;i<MAX_LOG_LISTENERS;i++) {
    _listeners[i]==NULL;
  }
  
  LogHandler::logHandler = new LogHandler();
  InputHandler::registerListener(LogHandler::logHandler);
}

String LogHandler::getName() {
  return LOG_HANDLER_MODULE_NAME;
}

bool LogHandler::onInput(String cmd) {
  String v1;
  int v2;
  
  if (InputHandler::parseParameters2(cmd, v1, v2)) {
    if (v1.equals(F("ENABLE"))) {
      LogHandler::doLog = v2==1 ? true : false;
      if (LogHandler::doLog) {
        logMsg(LOG_HANDLER_MODULE_NAME, F("Log enabled"));
      }
      return true;
    } else {
      return false;
    }
  } else {
    LogHandler::warning(LOG_HANDLER_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
    return false;
  }
}

void LogHandler::logMsg(String moduleName, String msg) {
  if (!doLog) return;

  String str = moduleName;
  str.concat(F("> "));
  str.concat(msg);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str);
}

void LogHandler::logMsg(String moduleName, String msg, int val) {
  if (!doLog) return;

  String str = moduleName;
  str.concat(F("> "));
  str.concat(msg);
  str.concat(F(" "));
  str.concat(val);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str);
}

void LogHandler::logMsg(String moduleName, String msg, int val1, int val2) {
  if (!doLog) return;

  String str = moduleName;
  str.concat(F("> "));
  str.concat(msg);
  str.concat(F(" "));
  str.concat(val1);
  str.concat(F(" "));
  str.concat(val2);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str);
}

void LogHandler::logMsg(String moduleName, String msg, String val) {
  if (!doLog) return;

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
  if (!doLog) return;

  String str = moduleName;
  str.concat(F("> WARN: "));
  str.concat(msg);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str, WARN);
}

void LogHandler::warning(String moduleName, String msg, int val) {
  if (!doLog) return;
  
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
  if (!doLog) return;

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
  if (!doLog) return;

  String str = moduleName;
  str.concat(F("> FATAL: "));
  str.concat(msg);

#ifdef DEBUG_PRINT
  DEBUG_SERIAL.println(str);
#endif

  sendToListeners(str, FATAL);
}

void LogHandler::sendToListeners(String msg, LogHandler::LOG_TYPE type) {
  if (!doLog) return;

  if (listenerCount>0) {
    for (uint8_t i=0;i<MAX_LOG_LISTENERS;i++) {
      if (_listeners[i]!=NULL) {
        _listeners[i]->onLogMessage(msg, type);
      }
    }
  }
}

