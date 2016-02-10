#ifndef log_handler_h
#define log_handler_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "globals.h"
#include "input_handler.h"

#define DEBUG_PRINT
#define DEBUG_SERIAL Serial

#define MAX_LOG_LISTENERS 4

#define LED_ERROR_INTERVAL_MS 200

#define LOG_HANDLER_MODULE_NAME MODNAME("LOG")

class LogHandler : public InputHandler::InputListener {
public:
  enum LOG_TYPE {
    LOG,
    WARN,
    FATAL
  };
  
  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);

  class LogListener {
    public:
      uint8_t getIndex() { return index; }
      void setIndex(uint8_t index) { this->index = index; }
      
      virtual void onLogMessage(String msg, LOG_TYPE type=LOG);
      virtual String getModuleNameFilter() { return ""; }
      
    private:
      uint8_t index;
  };
  
  static LogListener* _listeners[MAX_LOG_LISTENERS];
  static uint8_t listenerCount;
  static bool doLog;
  
  static void init();

  static void logMsg(String moduleName, String msg);

  static void logMsg(String moduleName, String msg, int val);

  static void logMsg(String moduleName, String msg, int val1, int val2);

  static void logMsg(String moduleName, String msg, String val);
  
  static void warning(String moduleName, String msg);

  static void warning(String moduleName, String msg, int val);

  static void warning(String moduleName, String msg, String val);

  static void fatal(String moduleName, String msg);
  
  static bool registerListener(LogHandler::LogListener* listener);
  
  static void unregisterListener(LogListener* listener);
  
  static void sendToListeners(String msg, LOG_TYPE type=LOG);

protected:
  static LogHandler* logHandler;

  
};

#endif
