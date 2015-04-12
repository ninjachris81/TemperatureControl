#ifndef log_handler_h
#define log_handler_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "led_logic.h"

#define DEBUG_PRINT
#define DEBUG_SERIAL Serial

#define MAX_LOG_LISTENERS 4

#define LED_ERROR_INTERVAL_MS 200

class LogHandler {
public:
  enum LOG_TYPE {
    LOG,
    WARN,
    FATAL
  };

  class LogListener {
    public:
      uint8_t getIndex() { return index; }
      void setIndex(uint8_t index) { this->index = index; }
      
      virtual void onMessage(String msg, LOG_TYPE type=LOG);
      virtual String getModuleNameFilter() { return ""; }
      
    private:
      uint8_t index;
  };
  
  static LogListener* _listeners[MAX_LOG_LISTENERS];
  static uint8_t listenerCount;
  static LedLogic *ledLogic;
  static bool hasFatalError;
  
  static void init(LedLogic *ledLogic);

  static void logMsg(String moduleName, String msg);

  static void logMsg(String moduleName, String msg, int val);

  static void logMsg(String moduleName, String msg, String val);
  
  static void warning(String moduleName, String msg);

  static void warning(String moduleName, String msg, int val);

  static void fatal(String moduleName, String msg);
  
  static bool registerListener(LogHandler::LogListener* listener);
  
  static void unregisterListener(LogListener* listener);
  
  static void sendToListeners(String msg, LOG_TYPE type=LOG);
  
};

#endif
