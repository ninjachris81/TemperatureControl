#include "format_utils.h"

void FormatUtils::formatTime(String &str, uint8_t hour, uint8_t minute, uint8_t second) {
  str.concat(addFrontPadding(hour, 2));
  str.concat(F(":"));
  str.concat(addFrontPadding(minute, 2));
  str.concat(F(":"));
  str.concat(addFrontPadding(second, 2));
}

String FormatUtils::formatTime(uint8_t hour, uint8_t minute, uint8_t second) {
  String tmp;
  formatTime(tmp, hour, minute, second);
  return tmp;
}

String FormatUtils::addFrontPadding(int number, uint8_t totalLength, char c) {
  String returnStr;
  returnStr.concat(number);
  while(returnStr.length()<totalLength) returnStr = c + returnStr;
  return returnStr;
}


