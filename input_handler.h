#ifndef input_handler_h
#define input_handler_h

#include "globals.h"

#define INPUT_HANDLER_MODULE_NAME F("IH")

#define ERROR_WHILE_PARSING_PARAMS F("Error while parsing parameters")

#define MAX_INPUT_LISTENERS 16

class InputHandler {
public:
  class InputListener {
    public:
      virtual String getName() = 0;
      virtual bool onInput(String cmd) = 0;
  };

  void init();
  
  void update();
  
  void executeCmd(String cmd);
  
  static void registerListener(InputListener* listener);
  
  static bool parseParameters2(String bufferStr, String &v1, String &v2);

  static bool parseParameters2(String bufferStr, String &v1, int &v2);

  static bool parseParameters2(String bufferStr, int &v1, int &v2);

  static bool parseParameters3(String bufferStr, int &v1, int &v2, int &v3);

private:
  String tmpBuffer;
  
  static InputListener* listeners[MAX_INPUT_LISTENERS];
  static uint8_t listenerCount;
  
};


#endif
