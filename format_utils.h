#ifndef format_utils_h
#define format_utils_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class FormatUtils {
  
public:
  static void formatTime(String &str, uint8_t hour, uint8_t minute, uint8_t second);
  
  static String formatTime(uint8_t hour, uint8_t minute, uint8_t second);

  static String addFrontPadding(int number, uint8_t totalLength, char c ='0');
};

#endif
