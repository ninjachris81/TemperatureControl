#include "input_handler.h"
#include "format_utils.h"
#include "globals.h"
#include "log_handler.h"

InputHandler::InputListener* InputHandler::listeners[MAX_INPUT_LISTENERS];
uint8_t InputHandler::listenerCount = 0;

void InputHandler::executeCmd(String cmd) {
  int i = cmd.indexOf(" ");
  
  if (i>=0) {
    String receiver = cmd.substring(0, i);
    InputListener *target = NULL;
    
    // search for target
    for (uint8_t o=0;o<listenerCount;o++) {
      if (listeners[o]->getName().equals(receiver)) {
        target = listeners[o];
        break;
      }
    }
    
    if (target!=NULL) {
      LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("Send command @ "), receiver);
      cmd = cmd.substring(i+1);
      cmd.trim();
      if (!target->onInput(cmd)) {
        LogHandler::warning(INPUT_HANDLER_MODULE_NAME, F("Inv command: "), cmd);
      }
    } else {
      LogHandler::warning(INPUT_HANDLER_MODULE_NAME, F("Recv inv: "), receiver);
    }
  } else {
    LogHandler::warning(INPUT_HANDLER_MODULE_NAME, F("Inv format "), cmd);
  }
}

void InputHandler::registerListener(InputListener* listener) {
  if (listenerCount==MAX_INPUT_LISTENERS) {
    LogHandler::warning(INPUT_HANDLER_MODULE_NAME, F("Max IH"), listener->getName());
    return;
  }
  
  listeners[listenerCount] = listener;
  listenerCount++;
}

bool InputHandler::parseParameters2(String bufferStr, String &v1, String &v2) {
  // 1 2
  if (bufferStr.length()<3) return false;
  int tmpIndex = bufferStr.indexOf(' ');
  if (tmpIndex==-1) return false;
  
  v1 = bufferStr.substring(0, tmpIndex);
  v2 = bufferStr.substring(tmpIndex + 1);
  
  return true;
}

bool InputHandler::parseParameters2(String bufferStr, String &v1, int &v2) {
  String v2Str;
  bool returnVal = parseParameters2(bufferStr, v1, v2Str);
  v2 = v2Str.toInt();
  return returnVal;
}

bool InputHandler::parseParameters2(String bufferStr, int &v1, int &v2) {
  String v1Str;
  String v2Str;
  bool returnVal = parseParameters2(bufferStr, v1Str, v2Str);
  v1 = v1Str.toInt();
  v2 = v2Str.toInt();
  return returnVal;
}

bool InputHandler::parseParameters3(String bufferStr, int &v1, int &v2, int &v3) {
  String v1Str;
  
  bool returnVal = parseParameters3(bufferStr, v1Str, v2, v3);
  v1 = v1Str.toInt();
  return returnVal;
}

bool InputHandler::parseParameters3(String bufferStr, String &v1, int &v2, int &v3) {
  // 1 2 3
  if (bufferStr.length()<5) return false;
  int tmpIndex1 = bufferStr.indexOf(' ');
  int tmpIndex2 = bufferStr.indexOf(' ', tmpIndex1+1);
  if (tmpIndex1==-1 || tmpIndex2==-1) return false;
  
  v1 = bufferStr.substring(0, tmpIndex1);
  v2 = bufferStr.substring(tmpIndex1 + 1, tmpIndex2).toInt();
  v3 = bufferStr.substring(tmpIndex2 + 1).toInt();
  
  return true;
}

/*
bool InputHandler::parseParameters4(String bufferStr, String &v1, int &v2, int &v3, int &v4) {
  // 1 2 3 4
  if (bufferStr.length()<7) return false;
  int tmpIndex1 = bufferStr.indexOf(' ');
  int tmpIndex2 = bufferStr.indexOf(' ', tmpIndex1+1);
  int tmpIndex3 = bufferStr.indexOf(' ', tmpIndex2+1);
  if (tmpIndex1==-1 || tmpIndex2==-1 || tmpIndex3==-1) return false;
  
  v1 = bufferStr.substring(0, tmpIndex1);
  v2 = bufferStr.substring(tmpIndex1 + 1, tmpIndex2).toInt();
  v3 = bufferStr.substring(tmpIndex2 + 1).toInt();
  v4 = bufferStr.substring(tmpIndex3 + 1).toInt();
  
  return true;
}
*/
