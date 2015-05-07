#include "input_handler.h"
#include "format_utils.h"

void InputHandler::init(Settings* settings, TemperatureLogic* temperatureLogic, TimeLogic* timeLogic, DisplayLogic* displayLogic) {
  Serial.begin(9600);
  this->settings = settings;
  this->temperatureLogic = temperatureLogic;
  this->timeLogic = timeLogic;
  this->displayLogic = displayLogic;
}

void InputHandler::update() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c!=';') {
      tmpBuffer.concat(c);
    } else {
      LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("EXEC "), tmpBuffer);
      if (tmpBuffer.equals(F("settings save"))) {
        this->settings->saveSettings();
      } else if (tmpBuffer.equals(F("settings default"))) {
        this->settings->defaultSettings();
      } else if (tmpBuffer.equals(F("time"))) {
        // read time
        LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("Current time is: "), FormatUtils::formatTime(RTC.hour, RTC.minute, RTC.second));
      } else if (tmpBuffer.startsWith(F("time"))) {
        int v1, v2, v3;
        if (parseParameters3(tmpBuffer, 5, v1, v2, v3)) {
          // set time
          timeLogic->save(v1, v2, v3);
        } else {
          LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
        }        
      } else if (tmpBuffer.equals(F("temperature"))) {
          // read temp
          LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("HC temperature is "), this->temperatureLogic->getTemperatureHC());
          LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("W temperature is "), this->temperatureLogic->getTemperatureW());
      } else if (tmpBuffer.startsWith(F("temperature"))) {
        int v1, v2;
        if (parseParameters2(tmpBuffer, 12, v1, v2)) {
          // set temp
          this->temperatureLogic->simulateTemperature(v1, v2);
        } else {
          LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
        }        
      } else if (tmpBuffer.startsWith(F("key"))) {
        int v1, v2;
        if (parseParameters2(tmpBuffer, 4, v1, v2)) {
          // set temp
          this->displayLogic->simulateKey(v1, v2);
        } else {
          LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, ERROR_WHILE_PARSING_PARAMS);
        }        
      }
      
      tmpBuffer = "";
    }
  }
}

bool InputHandler::parseParameters2(String &bufferStr, int index, int &v1, int &v2) {
  String tmp = bufferStr.substring(index);
  
  // 1 2
  if (tmp.length()<3) return false;
  int tmpIndex = tmp.indexOf(' ');
  if (tmpIndex==-1) return false;
  
  v1 = tmp.substring(0, tmpIndex).toInt();
  v2 = tmp.substring(tmpIndex + 1).toInt();
  
  return true;
}

bool InputHandler::parseParameters3(String &bufferStr, int index, int &v1, int &v2, int &v3) {
  String tmp = bufferStr.substring(index);
  
  // 1 2 3
  if (tmp.length()<5) return false;
  int tmpIndex1 = tmp.indexOf(' ');
  int tmpIndex2 = tmp.indexOf(' ', tmpIndex1+1);
  if (tmpIndex1==-1 || tmpIndex2==-1) return false;
  
  v1 = tmp.substring(0, tmpIndex1).toInt();
  v2 = tmp.substring(tmpIndex1 + 1, tmpIndex2).toInt();
  v3 = tmp.substring(tmpIndex2 + 1).toInt();
  
  return true;
}
