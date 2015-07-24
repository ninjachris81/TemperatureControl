#ifndef input_handler_h
#define input_handler_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define INPUT_HANDLER_MODULE_NAME F("IH")

#define ERROR_WHILE_PARSING_PARAMS F("Error while parsing parameters")

#define MAX_INPUT_LISTENERS 12

class InputHandler {
public:
  class InputListener {
    public:
      virtual String getName() = 0;
      virtual bool onInput(String cmd) = 0;
  };

  static void executeCmd(String cmd);
  
  static void registerListener(InputListener* listener);
  
  static bool parseParameters2(String bufferStr, String &v1, String &v2);

  static bool parseParameters2(String bufferStr, String &v1, int &v2);

  static bool parseParameters2(String bufferStr, int &v1, int &v2);

  static bool parseParameters3(String bufferStr, int &v1, int &v2, int &v3);

  static bool parseParameters3(String bufferStr, String &v1, int &v2, int &v3);

//  static bool parseParameters4(String bufferStr, String &v1, int &v2, int &v3, int &v4);

private:
  static InputListener* listeners[MAX_INPUT_LISTENERS];
  static uint8_t listenerCount;
  
};


#endif
