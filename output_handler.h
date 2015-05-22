#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MAX_OUTPUT_SINKS 1

class OutputHandler {
public:
  class OutputSink {
    public:
      virtual void sendCmd(String sender, String cmd) = 0;
  };
  
  static void sendCmd(String sender, String cmd);
  
  static void registerSink(OutputSink* outputSink);
  
  static OutputSink* sinks[MAX_OUTPUT_SINKS];
  static uint8_t sinkCount;
  
};
