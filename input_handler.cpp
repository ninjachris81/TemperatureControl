#include "input_handler.h"
#include "format_utils.h"

InputHandler::InputListener* InputHandler::listeners[MAX_INPUT_LISTENERS];
uint8_t InputHandler::listenerCount = 0;


void InputHandler::init() {
  Serial.begin(9600);
}

void InputHandler::update() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c!=';') {
      tmpBuffer.concat(c);
    } else {
      int i = tmpBuffer.indexOf(" ");
      
      if (i>=0) {
        String receiver = tmpBuffer.substring(0, i);
        InputListener *target = NULL;
        
        for (uint8_t o=0;o<listenerCount;o++) {
          if (listeners[o]->getName().equals(receiver)) {
            target = listeners[o];
            break;
          }
        }
        
        if (target!=NULL) {
          LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("Sending command to "), receiver);
          tmpBuffer = tmpBuffer.substring(i+1);
          tmpBuffer.trim();
          if (!target->onInput(tmpBuffer)) {
            LogHandler::warning(INPUT_HANDLER_MODULE_NAME, F("Invalid command: "), tmpBuffer);
          }
        } else {
          LogHandler::warning(INPUT_HANDLER_MODULE_NAME, F("Receiver is invalid: "), receiver);
        }
      } else {
        LogHandler::warning(INPUT_HANDLER_MODULE_NAME, F("Invalid command format "), tmpBuffer);
      }

      tmpBuffer = "";
    }
  }
}

void InputHandler::registerListener(InputListener* listener) {
  listeners[listenerCount] = listener;
  listenerCount++;
}

bool InputHandler::parseParameters2(String bufferStr, int &v1, int &v2) {
  // 1 2
  if (bufferStr.length()<3) return false;
  int tmpIndex = bufferStr.indexOf(' ');
  if (tmpIndex==-1) return false;
  
  v1 = bufferStr.substring(0, tmpIndex).toInt();
  v2 = bufferStr.substring(tmpIndex + 1).toInt();
  
  return true;
}

bool InputHandler::parseParameters3(String bufferStr, int &v1, int &v2, int &v3) {
  // 1 2 3
  if (bufferStr.length()<5) return false;
  int tmpIndex1 = bufferStr.indexOf(' ');
  int tmpIndex2 = bufferStr.indexOf(' ', tmpIndex1+1);
  if (tmpIndex1==-1 || tmpIndex2==-1) return false;
  
  v1 = bufferStr.substring(0, tmpIndex1).toInt();
  v2 = bufferStr.substring(tmpIndex1 + 1, tmpIndex2).toInt();
  v3 = bufferStr.substring(tmpIndex2 + 1).toInt();
  
  return true;
}
